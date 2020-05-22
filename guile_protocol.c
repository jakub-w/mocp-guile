#include "guile_protocol.h"

#include <errno.h>

#include "common.h"
#include "log.h"

void guile_event_queue_init () {
	guile_events.mutex = scm_make_mutex ();
	guile_events.head = SCM_EOL;
	guile_events.tail = SCM_UNDEFINED;

	guile_events.wake_up_pending = 0;

	if (pipe (guile_events.wake_up_pipe) < 0) {
		fatal ("pipe() failed: %s", strerror (errno));
	}
}

void guile_event_push (SCM proc, SCM args) {
	SCM_ASSERT_TYPE (scm_procedure_p (proc), proc, SCM_ARG1,
			 "guile_event_push", "procedure");
	SCM_ASSERT_TYPE (scm_pair_p (args), args, SCM_ARG2,
			 "guile_event_push", "list of arguments");

	scm_dynwind_begin(0);
	scm_dynwind_lock_mutex (guile_events.mutex);

	if (scm_is_null (guile_events.head)) {
		guile_events.head = scm_list_1 (scm_cons (proc, args));
		guile_events.tail = guile_events.head;
	}
	else {
		scm_set_cdr_x (guile_events.tail,
			       scm_list_1 (scm_cons (proc, args)));
		guile_events.tail = scm_cdr (guile_events.tail);
	}

	int w = 1;
	if (!guile_events.wake_up_pending) {
		debug ("Waking up the interface to handle Guile request");
		if (write (guile_events.wake_up_pipe[1], &w, sizeof(w)) < 0) {
			log_errno ("Can't sent the wake up signal: "
				   "(write() failed)",
			   errno);
		}

	}

	scm_dynwind_end ();
}

SCM guile_event_pop_unsafe () {
	SCM result = SCM_BOOL_F;

	if (!scm_is_null (guile_events.head)) {
		result = scm_car (guile_events.head);
		guile_events.head = scm_cdr (guile_events.head);

		if (scm_is_null (guile_events.head)) {
			guile_events.tail = SCM_UNDEFINED;
		}
	}

	return result;
}

SCM guile_event_pop () {
	SCM result = SCM_BOOL_F;

	scm_dynwind_begin (0);
	scm_dynwind_lock_mutex (guile_events.mutex);

	result = guile_event_pop_unsafe ();

	scm_dynwind_end ();

	return result;
}

void* guile_consume_wake_up (void* result) {
	int w;

	scm_dynwind_begin (0);
	scm_dynwind_lock_mutex (guile_events.mutex);

	*((int*) result) =
		(read (guile_events.wake_up_pipe[0], &w, sizeof(w)) >= 0);

	guile_events.wake_up_pending = 0;

	scm_dynwind_end ();

	return result;
}
