#ifndef GUILE_H
#define GUILE_H

#include <libguile.h>

#ifdef __cplusplus
extern "C" {
#endif

void guile_init (const char* init_file);
void guile_cleanup ();

// Returns a string that must be free()'d.
void* guile_get_lyrics (void* filename);

#ifdef __cplusplus
}
#endif

#endif // GUILE_H
