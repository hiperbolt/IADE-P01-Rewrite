/* ist1102416 - Tomás Simões */

#ifndef MAIN_H
#define MAIN_H

/* 
    Arguments 
*/
#define MAX_NUMBER_OF_ARGUMENTS 41 /* e.g. l with all possible airports */
#define MAX_ARGUMENT_SIZE 200


/* 
    Date
*/

/* Unix Epoch */
#define EPOCH_DAY 1
#define EPOCH_MONTH 1
#define EPOCH_YEAR 1970

/* Starting Date - 1/01/2022 */
#define START_EPOCH 455520 /* Calculated with the get_epoch() function in proj1.c */
#define START_DAY 1
#define START_MONTH 1
#define START_YEAR 2022

/* Max Possible Date - 31/12/2023 */
#define MAX_EPOCH 473184 /* Calculated with the get_epoch() function in proj1.c */
#define MAX_DAY 31
#define MAX_MONTH 12
#define MAX_YEAR 2023


/*
    Airports
*/
#define MAX_AIRPORTS 40
#define IDENTIFIER_CHARS 4 /* 3 chars + terminating */
#define COUNTRY_CHARS 31 /* 30 chars + terminating */
#define CITY_CHARS 51 /* 50 chars + terminating */


/*
    Flights
*/
#define FLIGHT_CODE_CHARS 7 /* 6 chars + terminating */
#define MIN_FLIGHT_CODE_NUM 1
#define MAX_FLIGHT_CODE_NUM 9999
#define MAX_FLIGHTS 30000
#define MIN_PASSENGERS 10
#define MAX_PASSENGERS 100
#define MAX_DURATION 12 /* The max duration of a flight is 12 hours */


/*
    Structs
*/

typedef struct
{
    int day;
    int month;
    int year;
} date;

typedef struct
{
    int hour;
    int min;
} time;

typedef struct
{
    char id[IDENTIFIER_CHARS];
    char country[COUNTRY_CHARS];
    char city[CITY_CHARS];
    /*  Tt's computationally more
        economic to keep track of this value everytime a flight is added
        than to calculate it later on.
        This optimization is highly significative.
    */
    int flights_departing_from_here;
} airport;

typedef struct
{
    char flight_code[FLIGHT_CODE_CHARS];
    airport * departure_airport;
    airport * arrival_airport;
    date departure_date;
    time departure_time;
    time duration;
    int max_passengers;
    date arrival_date;
    time arrival_time;
} flight;

typedef struct
{
    airport airports[MAX_AIRPORTS];
    int airports_counter;
    flight flights[MAX_FLIGHTS];
    int flights_counter;
    date current_date;

} state;


/*
    proj1.c
*/
int parser();
int handler(char c, char arguments[MAX_NUMBER_OF_ARGUMENTS][MAX_ARGUMENT_SIZE]);
void add_airport(char arguments[MAX_NUMBER_OF_ARGUMENTS][MAX_ARGUMENT_SIZE]);
void list_airport(char (*arguments)[MAX_ARGUMENT_SIZE]);
void sort_all_airports(char (*arguments)[MAX_ARGUMENT_SIZE]);
void add_or_list_flights(char (*arguments)[MAX_ARGUMENT_SIZE]);
void list_flights(char (*arguments)[MAX_ARGUMENT_SIZE]);
void add_flight(char (*arguments)[MAX_ARGUMENT_SIZE]);
void departure_flights(char (*arguments)[MAX_ARGUMENT_SIZE]);

int too_many_flights();
int check_flight_exists(char flight_code[FLIGHT_CODE_CHARS], date departure_date);
int check_flight_code(char flight_code[FLIGHT_CODE_CHARS]);
void print_airport_list(char (*arguments)[MAX_ARGUMENT_SIZE]);
int error_helper_noSuchAirportID(char airport_id[IDENTIFIER_CHARS]);
airport *helper_find_airport(char airport_id[IDENTIFIER_CHARS]);
void sort_array(void *array, unsigned int size, unsigned  int element_count, long (*compare)(const void *, const void *));
void merge(void *array, unsigned int size, unsigned int left_size, unsigned  int right_size,  long (*compare)(const void *, const void *));
long flights_code_compare(const void * airport1, const void * airport2);
int check_same_date(date d1, date d2);
void date_to_human(char *buffer, date *date_strct);
void time_to_human(char *buffer, time *time_strct);
void human_to_date(char *date_human, date *date_buffer);
void human_to_time(char *time_human, time *time_buffer);
int helper_get_datetime(date d, time t);
int validate_date(date new_date);
int convert_to_epoch(date d, int add_to_year);


#endif