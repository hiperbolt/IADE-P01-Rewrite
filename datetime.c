/* ist1102416 - Tomás Simões */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj1.h"

int check_same_date(date d1, date d2) {
    /*  Returns 0 if same date, 1 if not.
        (mainly because normally being the
        same date is undesirable)
    */
    if (d1.day == d2.day && d1.month == d2.month && d1.year == d2.year)
    {
        return 0;
    }
    return 1;
}

void date_to_human(char *buffer, date *date_strct) {
    /*  Converts a date struct to a human readable string */
    sprintf(buffer, "%02d-%02d-%02d", date_strct->day, date_strct->month, date_strct->year);
}

void time_to_human(char *buffer, time *time_strct) {
    /*  Converts a time struct to a human readable string */
    sprintf(buffer, "%02d:%02d", time_strct->hour, time_strct->min);
}

void human_to_date(char *date_human, date *date_buffer) {
    /*  
        Converts a human readable date string to a date struct
    
        From the man page:
        "sscanf() reads its input from the character string pointed to by str"

        Therefor, we can use the same formatting options we would with
        scanf() and dump the result directly into a buffer.

        This is a much more compact and efficient way of converting the date.
    */
    sscanf(date_human, "%d-%d-%d", &date_buffer->day, &date_buffer->month, &date_buffer->year);
}

void human_to_time(char *time_human, time *time_buffer) {
    /*  
        Converts a human readable time string to a time struct
    */
    sscanf(time_human, "%d:%d", &time_buffer->hour, &time_buffer->min);
}

int helper_get_datetime(date d, time t) {
    /* 
        Calculates AAAAMMDDHHMM from a date and time struct.
    */
    return ((d.year * 100000000) + (d.month * 1000000) + (d.day * 10000) + (t.hour * 100) + t.min);
}

int validate_date(date new_date) {
    /*  
        Helper function that guarantees that the date isn't in the past, and isn't more
        than one year in the future.
        Returns 1 if valid, 0 if invalid.
    */
    int new_epoch, current_epoch, year_ahead_epoch;

    new_epoch = convert_to_epoch(new_date, 0);
    current_epoch = convert_to_epoch(global.current_date, 0);
    year_ahead_epoch = convert_to_epoch(global.current_date, 1);

    if (new_epoch < START_EPOCH || new_epoch < current_epoch || new_epoch > year_ahead_epoch || new_epoch > MAX_EPOCH)
    {   
        printf("invalid date\n");
        return 0;
    }
    return 1;

}

int convert_to_epoch(date d, int add_to_year) {
    /* Converts date struct to a made up epoch standard */
    int day = d.day;
    int m = d.month;
    int y = d.year + add_to_year;
    int epoch_time = 0;

    for (a = EPOCH_YEAR; a < y; a++)
    {
        epoch_time += 8760;
    }
    for (a = EPOCH_MONTH; a < m; a++)
    {
        epoch_time += 720; 
    }
    for (a = EPOCH_DAY; a < day; a++)
    {
        epoch_time += 24;
    }
    return epoch_time;
}

void calculate_arrival_datetime(date* date_buffer, time* time_buffer, date dep_date, time dep_time, time duration) {
    /* This function should eventually be optimized */
    int departure_year = dep_date.year;
    int departure_month = dep_date.month;
    int departure_day = dep_date.day;
    int departure_hour = dep_time.hour;
    int departure_min = dep_time.min;
    int duration_hour = duration.hour;
    int duration_min = duration.min;
    int carry = 0;
    int currentMonthDays = current_month_days(departure_month);

    departure_min += duration_min;
    if (departure_min >= 60)
    {
        carry = 1;
        departure_min = departure_min % 60;
    }
    
    departure_hour += duration_hour + carry;
    carry = 0;
    if (departure_hour >= 24)
    {
        carry = 1;
        departure_hour = departure_hour % 24;
    }

    departure_day += carry;
    carry = 0;

    if (departure_day > currentMonthDays) {
        carry = 1;
        departure_day = departure_day % currentMonthDays;
    }

    departure_month += carry;
    carry = 0;
    if (departure_month > 12)
    {
        carry = 1;
        departure_month = 1;
    }

    departure_year += carry;
    carry = 0;

    date_buffer->day = departure_day;
    date_buffer->month = departure_month;
    date_buffer->year = departure_year;
    time_buffer->hour = departure_hour;
    time_buffer->min = departure_min;
}

int current_month_days(int m){
    /* Returns the number of days in the current month */
    if (m == 2)
    {
        return 28;
    }
    if (m < 8)
    {
        if (m%2 == 0)
        {
            return 30;
        } else{
            return 31;
        }
    } else {
        if (m%2 == 0)
        {
            return 31;
        } else {
            return 30;
        }
    }
}