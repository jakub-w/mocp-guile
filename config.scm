(use-modules (ice-9 popen)
	     (ice-9 regex)
	     ((ice-9 i18n) #:select (string-locale-titlecase))
	     ((ice-9 rdelim) #:select (read-line))
	     ((srfi srfi-1) #:select (last))
	     ((srfi srfi-2) #:select (and-let*)))

(define lyrics-dir (string-append (getenv "HOME") "/.lyrics/"))

(define (make-tags artist title)
  (list artist title))
(define (tags-artist tags)
  (car tags))
(define (tags-title tags)
  (cadr tags))

(define (file-extension filename)
  (last (string-split filename #\.)))

(define (process-lines command . args)
  (let ((port #f))
    (dynamic-wind
      (lambda ()
	(set! port (apply open-pipe* "r" command args)))
      (lambda ()
	(let loop ((line (read-line port))
		   (lines '()))
	  (if (eof-object? line)
	      (reverse! lines)
	      (loop (read-line port) (cons line lines)))))
      (lambda ()
	(close-pipe port)))))


(define (get-mp3-tags filename)
  (let* ((lines (process-lines "id3info" filename))
	 (raw-tags
	  (filter
	   identity
	   (map (lambda (line)
		  (and-let* ((m (string-match "=== ([0-9A-Z]+) [^:]+: (.*)"
					      line))
			     (type (match:substring m 1))
			     (content (match:substring m 2)))
		    (cons type content)))
		lines)))
	 (artist (or (assoc-ref raw-tags "TPE1")
		     (assoc-ref raw-tags "TPE2")))
	 (title (assoc-ref raw-tags "TIT2")))
    (and artist
	 title
	 (make-tags artist title))))

(define (get-flac-tags filename)
  (let* ((lines (process-lines "metaflac"
			       "--list"
			       "--block-type=VORBIS_COMMENT"
			       filename))
	 (raw-tags
	  (filter
	   identity
	   (map (lambda (line)
		  (and-let* ((m (string-match ".*: ([A-Za-z]+)=(.*)" line))
			     (type (match:substring m 1))
			     (content (match:substring m 2)))
		    (cons type content)))
		lines)))
	 (artist (or (assoc-ref raw-tags "ARTIST")
		     (assoc-ref raw-tags "Artist")
		     (assoc-ref raw-tags "artist")))
	 (title (or (assoc-ref raw-tags "TITLE")
		    (assoc-ref raw-tags "Title")
		    (assoc-ref raw-tags "title"))))
    (and artist title
	 (make-tags artist title))))

(define tags-functions
  `(("mp3" . ,get-mp3-tags)
    ("flac" . ,get-flac-tags)))

(define (get-lyrics-file filename)
  (catch #t
    (lambda ()
      (and-let* ((fun (assoc-ref tags-functions (file-extension filename)))
		 (tags (fun filename))
		 (lyrics-file
		  (format #f "~a/~a - ~a.txt"
			  lyrics-dir
			  (string-locale-titlecase (tags-artist tags))
			  (string-locale-titlecase (tags-title tags)))))
	(when (file-exists? lyrics-file)
	  lyrics-file)))
    (lambda (key . args)
      (apply display-error #f (current-error-port) args))))


(add-hook! get-lyrics-functions get-lyrics-file)
