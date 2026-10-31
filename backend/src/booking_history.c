#include <string.h>
#include "booking_history.h"
#include "railway.h"

int booking_history_record_event(BookingEvent *events,
                                 size_t *event_count,
                                 size_t max_events,
                                 unsigned long *next_sequence,
                                 const char *event_type,
                                 unsigned long booking_id,
                                 int train_number)
{
    if (events == NULL || event_count == NULL || next_sequence == NULL || event_type == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    if (*event_count >= max_events) {
        return RAILWAY_ERR_CAPACITY_FULL;
    }

    BookingEvent *ev = &events[*event_count];
    ev->sequence = (*next_sequence)++;
    ev->booking_id = booking_id;
    ev->train_number = train_number;

    strncpy(ev->event_type, event_type, EVENT_LEN - 1);
    ev->event_type[EVENT_LEN - 1] = '\0';

    (*event_count)++;
    return RAILWAY_SUCCESS;
}
