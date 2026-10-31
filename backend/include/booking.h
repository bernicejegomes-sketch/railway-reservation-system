#ifndef BOOKING_H
#define BOOKING_H

#include <stddef.h>

#define MAX_BOOKINGS 500
#define NAME_LEN 80
#define ROUTE_LEN 80
#define TIME_LEN 16
#define DATE_LEN 11
#define EVENT_LEN 24

/* Booking status enumeration */
typedef enum {
    BOOKING_CONFIRMED = 0,
    BOOKING_WAITING   = 1,
    BOOKING_CANCELLED = 2
} BookingStatus;

/* Central passenger booking record */
typedef struct {
    unsigned long booking_id;        /* Unique strictly monotonic booking identifier */
    unsigned long created_sequence;  /* Monotonic sequence preserving chronological creation order */
    char passenger_name[NAME_LEN];   /* Passenger full name */
    int age;                         /* Passenger age in years (1 to 120) */
    char journey_date[DATE_LEN];     /* ISO date format YYYY-MM-DD */
    int train_number;                /* Assigned train number */
    int seat_number;                 /* 1-based index; 0 indicates no physical seat allocated */
    BookingStatus status;            /* Current lifecycle status */
} Booking;

#endif /* BOOKING_H */
