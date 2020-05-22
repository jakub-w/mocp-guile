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
} guile_events;

void guile_event_queue_init ();

void guile_event_push (SCM proc, SCM args);
SCM guile_event_pop ();

#endif // HAVE_GUILE

#ifdef __cplusplus
}
#endif

#endif // GUILE_PROTOCOL_H
