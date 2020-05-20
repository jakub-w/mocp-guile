#include "guile_protocol.h"

void guile_event_queue_init () {
	guile_events.mutex = scm_make_mutex ();
	guile_events.head = SCM_EOL;
	guile_events.tail = SCM_UNDEFINED;
}

void guile_event_push (SCM thunk) {
	SCM_ASSERT_TYPE(scm_thunk_p(thunk), thunk, SCM_ARG1,
			"guile_event_push", "thunk");

	scm_dynwind_begin(0);
	scm_dynwind_lock_mutex (guile_events.mutex);

	if (scm_is_null (guile_events.head)) {
		guile_events.head = scm_list_1 (thunk);
		guile_events.tail = guile_events.head;
	}
	else {
		scm_set_cdr_x (guile_events.tail, scm_list_1 (thunk));
		guile_events.tail = scm_cdr (guile_events.tail);
	}

	scm_dynwind_end ();
}

SCM guile_event_pop () {
	SCM result = SCM_BOOL_F;

	scm_dynwind_begin (0);
	scm_dynwind_lock_mutex (guile_events.mutex);

	if (!scm_is_null (guile_events.head)) {
		result = scm_car (guile_events.head);
		guile_events.head = scm_cdr (guile_events.head);

		if (scm_is_null (guile_events.head)) {
			guile_events.tail = SCM_UNDEFINED;
		}
	}

	scm_dynwind_end ();

	return result;
}
