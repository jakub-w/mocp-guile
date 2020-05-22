#ifndef GUILE_PROTOCOL_H
#define GUILE_PROTOCOL_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_GUILE
# include "guile.h"
#endif // HAVE_GUILE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_GUILE

struct {
	SCM head;
	SCM tail;
	SCM mutex;

	/* Pipe used to wake up the interface_loop() from guile code */
	int wake_up_pipe[2];
	int wake_up_pending;
} guile_events;

void guile_event_queue_init ();

/* All of the following functions must be called from inside the guile mode */
void guile_event_push (SCM proc, SCM args);
/* Doesn't lock the queue, beware. */
SCM guile_event_pop_unsafe ();
SCM guile_event_pop ();

void* guile_consume_wake_up (void* result);

#endif // HAVE_GUILE

#ifdef __cplusplus
}
#endif

#endif // GUILE_PROTOCOL_H
