(use-modules ((ice-9 i18n) #:select (string-locale-titlecase)))

(define lyrics-dir (string-append (getenv "HOME") "/.lyrics/"))

(define (get-lyrics-file filename)
  (catch #t
    (lambda ()
      (let* ((tags (get-file-tags filename))
	     (artist (file-tags-artist tags))
	     (title (file-tags-title tags))
	     (lyrics-file
	      (format #f "~a/~a - ~a.txt"
		      lyrics-dir
		      (string-locale-titlecase artist)
		      (string-locale-titlecase title))))
	(when (file-exists? lyrics-file)
	  lyrics-file)))
    (lambda (key . args)
      (apply display-error #f (current-error-port) args))))


(add-hook! get-lyrics-functions get-lyrics-file)
