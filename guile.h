#ifndef GUILE_H
#define GUILE_H

#include <libguile.h>

#ifdef __cplusplus
extern "C" {
#endif

void guile_init (const char* init_file);
void guile_cleanup ();

SCM guile_run_hook_until_success (SCM hook, SCM args);

#ifdef __cplusplus
}
#endif

#endif // GUILE_H
