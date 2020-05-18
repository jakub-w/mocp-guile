#ifndef  LYRICS_H
#define  LYRICS_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_GUILE
# include <libguile.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_GUILE
SCM guile_get_lyrics_functions;
#endif

typedef lists_t_strs *lyrics_t_formatter (lists_t_strs *lines, int height, int width, void *data);
typedef void lyrics_t_reaper (void *data);

lists_t_strs *lyrics_lines_get (void);
void lyrics_lines_set (lists_t_strs *lines);
lists_t_strs *lyrics_load_file (const char *filename);
void lyrics_autoload (const char *filename);
void lyrics_use_formatter (lyrics_t_formatter, lyrics_t_reaper, void *data);
lists_t_strs *lyrics_format (int height, int width);
void lyrics_cleanup (void);

#ifdef __cplusplus
}
#endif

#endif
