#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "validation.h"
#include "booking.h"
#include "train.h"

int validate_passenger_name(const char *name)
{
    if (name == NULL) {
        return 0;
    }

    size_t len = strlen(name);
    if (len == 0 || len >= NAME_LEN) {
        return 0;
    }

    /* Verify name is not solely whitespace and contains valid name characters */
    int non_space_found = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)name[i];
        if (!isspace(c)) {
            non_space_found = 1;
        }
        /* Allow alphabet, spaces, hyphens, periods, apostrophes */
        if (!isalpha(c) && !isspace(c) && c != '-' && c != '.' && c != '\'') {
            return 0;
        }
    }

    return non_space_found;
}

int validate_age(int age)
{
    return (age >= 1 && age <= 120);
}

static int is_leap_year(int year)
{
    if (year % 400 == 0) return 1;
    if (year % 100 == 0) return 0;
    return (year % 4 == 0);
}

int validate_journey_date(const char *date)
{
    if (date == NULL) {
        return 0;
    }

    /* Format must be exactly YYYY-MM-DD: 10 characters */
    if (strlen(date) != 10) {
        return 0;
    }

    if (date[4] != '-' || date[7] != '-') {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) {
            return 0;
        }
    }

    int year = 0, month = 0, day = 0;
    if (sscanf(date, "%4d-%2d-%2d", &year, &month, &day) != 3) {
        return 0;
    }

    if (year < 2024 || year > 2100) {
        return 0;
    }

    if (month < 1 || month > 12) {
        return 0;
    }

    static const int days_in_month[] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    int max_days = days_in_month[month];
    if (month == 2 && is_leap_year(year)) {
        max_days = 29;
    }

    if (day < 1 || day > max_days) {
        return 0;
    }

    return 1;
}

int validate_train_capacity(int capacity)
{
    return (capacity >= 1 && capacity <= MAX_SEATS_PER_TRAIN);
}

int validate_train_number(int train_number)
{
    return (train_number > 0);
}

int validate_booking_id(unsigned long booking_id)
{
    return (booking_id > 0);
}
