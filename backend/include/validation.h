#ifndef VALIDATION_H
#define VALIDATION_H

/* Input validation helper functions */
int validate_passenger_name(const char *name);
int validate_age(int age);
int validate_journey_date(const char *date);
int validate_train_capacity(int capacity);
int validate_train_number(int train_number);
int validate_booking_id(unsigned long booking_id);

#endif /* VALIDATION_H */
