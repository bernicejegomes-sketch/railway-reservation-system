#include <string.h>
#include <stdio.h>
#include "train.h"
#include "railway.h"
#include "validation.h"

int train_init(Train *train,
               int train_number,
               const char *name,
               const char *source,
               const char *dest,
               const char *dep_time,
               int capacity)
{
    if (train == NULL || !validate_train_number(train_number) || !validate_train_capacity(capacity)) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    if (name == NULL || source == NULL || dest == NULL || dep_time == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    memset(train, 0, sizeof(Train));

    train->train_number = train_number;
    train->capacity = capacity;

    /* Safe bounded string copies */
    strncpy(train->train_name, name, NAME_LEN - 1);
    train->train_name[NAME_LEN - 1] = '\0';

    strncpy(train->source, source, ROUTE_LEN - 1);
    train->source[ROUTE_LEN - 1] = '\0';

    strncpy(train->destination, dest, ROUTE_LEN - 1);
    train->destination[ROUTE_LEN - 1] = '\0';

    strncpy(train->departure_time, dep_time, TIME_LEN - 1);
    train->departure_time[TIME_LEN - 1] = '\0';

    /* Initialize seat states to 0 (all vacant) */
    for (int i = 0; i < MAX_SEATS_PER_TRAIN; i++) {
        train->seats[i] = 0;
    }

    confirmed_list_init(&train->confirmed);
    waiting_queue_init(&train->waiting);

    return RAILWAY_SUCCESS;
}

void train_cleanup(Train *train)
{
    if (train == NULL) {
        return;
    }

    confirmed_list_clear(&train->confirmed);
    waiting_queue_clear(&train->waiting);

    train->capacity = 0;
    train->train_number = 0;
    memset(train->seats, 0, sizeof(train->seats));
}
