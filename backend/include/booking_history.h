#ifndef BOOKING_HISTORY_H
#define BOOKING_HISTORY_H

#include <stddef.h>
#include "booking.h"

#define MAX_EVENTS 2000

/* Event record */
typedef struct {
    unsigned long sequence;          /* Monotonic event counter */
    char event_type[EVENT_LEN];      /* Event description (e.g. "BOOKING_CREATED", "TICKET_CANCELLED", "PROMOTED") */
    unsigned long booking_id;        /* Associated booking identifier */
    int train_number;                /* Associated train number */
} BookingEvent;

/* History management operations */
int booking_history_record_event(BookingEvent *events,
                                 size_t *event_count,
                                 size_t max_events,
                                 unsigned long *next_sequence,
                                 const char *event_type,
                                 unsigned long booking_id,
                                 int train_number);

#endif /* BOOKING_HISTORY_H */
