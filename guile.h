#ifndef GUILE_H
#define GUILE_H

#include <libguile.h>

#ifdef __cplusplus
extern "C" {
#endif


#define GUILE_ASSERT_SERVER()						\
	do { if (SCM_UNLIKELY (!(is_server ())))			\
			scm_error (guile_wrong_context_error, FUNC_NAME, \
				   "Can run only on server",		\
				   SCM_EOL, SCM_EOL);			\
	} while (0);

#define GUILE_ASSERT_CLIENT()						\
	do { if (SCM_UNLIKELY (is_server ()))				\
			scm_error (guile_wrong_context_error, FUNC_NAME, \
				   "Can run only on client",		\
				   SCM_EOL, SCM_EOL);			\
	} while (0);


SCM guile_wrong_context_error;


void guile_init (const char* init_file);
void guile_cleanup ();

SCM guile_run_hook_until_success (SCM hook, SCM args);

#ifdef __cplusplus
}
#endif

#endif // GUILE_H
