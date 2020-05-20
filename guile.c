#include "guile.h"

#include <libguile.h>
#include <libguile/hooks.h>

#include "common.h"
#include "files.h"
#include "log.h"

// Sources for snarfed bindings
#include "interface.h"
#include "interface_elements.h"
#include "lyrics.h"
#include "playlist.h"

SCM_GLOBAL_SYMBOL (guile_wrong_context_error, "wrong-context-error");

SCM_DEFINE (guile_run_hook_until_success, "run-hook-until-success", 1, 0, 1,
	    (SCM hook, SCM args),
	    "Apply all procedures from the hook @var{hook} to the arguments\n"
	    "@var{args} one after another until one of them doesn't return\n"
	    "#f.  The order of the procedure application is first to last.\n"
	    "The return value is the return value of the procedure that\n"
	    "succeeded or #f if none of them did.")
#define FUNC_NAME s_guile_run_hook_until_success
{
	SCM_VALIDATE_HOOK(SCM_ARG1, hook);
	if (scm_ilength (args) != SCM_HOOK_ARITY (hook)) {
		SCM_MISC_ERROR (
			"Hook ~S requires ~A arguments",
			scm_list_2 (hook,
				    scm_from_int (SCM_HOOK_ARITY (hook))));
	}

	SCM procs = SCM_HOOK_PROCEDURES (hook);
	SCM result;
	while (scm_is_pair (procs)) {
		result = scm_apply_0 (SCM_CAR (procs), args);
		if (scm_is_true (result))
			return result;
		procs = SCM_CDR (procs);
	}

	return SCM_BOOL_F;
}
#undef FUNC_NAME

void* guile_init_internal (void* init_file) {
#ifndef SCM_MAGIC_SNARFER
#include "guile.x"
#endif

	guile_init_interface ();
	guile_init_interface_elements ();
	guile_init_playlist ();
	guile_init_lyrics ();

	if (NULL == init_file || !can_read_file(init_file)) {
		return NULL;
	}

	scm_c_primitive_load ((const char*)init_file);

	return NULL;
}

void guile_init (const char* init_file) {
	scm_with_guile (&guile_init_internal, (void*)init_file);

	logit ("Guile initialized");
}

void guile_cleanup () {
	scm_run_finalizers();
}
