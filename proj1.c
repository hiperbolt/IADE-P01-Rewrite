/* ist1102416 - Tomás Simões */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj1.h"

/*  We declare some global variables, specifically:
    - i and j as size_t, for various instances of iteration;
    - a as int, for instances of iteration where an *actual* int type is needed;
    - state, which will hold the current system state;
*/
size_t i, j;
int a;

state global = {0};

int main() {
    global.current_date.day = START_DAY;
    global.current_date.month= START_MONTH;
    global.current_date.year = START_YEAR;

    /*
        We keep calling the handler function until 0 is returned.
        Then we cleanly exit.
    */
    while(parser())
    {
    }
    return 0;
}

int parser() {
    /*
        We create a 2d array for the arguments.
        This way arguments[0] will be the command,
        arguments[1] the first argument, etc...
    */
    char arguments[MAX_NUMBER_OF_ARGUMENTS][MAX_ARGUMENT_SIZE] = {0};

    /*  
        We assume no input will ever be larger than 1000 chars 
        1000 for the limit was an arbitrary decision.
    */
    char input_array[1000] = {0};

    /* We create a char pointer for strtok to use */
    char *pch;

    fgets(input_array, 200, stdin);
        
    /*  pch (pointer char) points at the current strtok token
        and is updated until strtok returns a null token */
    pch = strtok(input_array, " \n\t");
    for (a = 0; pch != NULL; a++)
    {   
        strcpy(arguments[a], pch);
        pch = strtok(NULL, " \n\t");
    }
    handler(input_array[0], arguments);
}

int handler (char c, char (*arguments)[MAX_ARGUMENT_SIZE]) {
    switch (c)
    {
        case 'q':
            return 0;
        case 'a':
            add_airport(arguments);
            return 1;
        case 'l':
            list_airport(arguments);
            return 1;
        case 'v':
            add_or_list_flights(arguments);
            return 1;
        case 'p':
            departure_flights(arguments);
            return 1;
    }
}

void add_airport(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    char city[CITY_CHARS];
    /*  
        id = arguments[1]
        country = arguments[2]
        (we do some stuff like this for compiler optimization reasons)
    */
    
    strcpy(city, arguments[3]);
    a = 4;
    while (arguments[a][0] != 0)
    {
        strncat(city, " ", 2);
        strncat(city, arguments[a], strlen(arguments[a]) + 1); /* +1 to account for terminating char */
        a++;
    }

    /* Check for exceptions */
    for (i = 0; i < IDENTIFIER_CHARS; i++)
    {
        if (islower(arguments[1][i]) != 0)
        {
            printf("invalid airport ID\n");
            return;
        }
    }

    if (global.airports_counter + 1 > MAX_AIRPORTS)
    {
        printf("too many airports\n");
        return;
    }

    for (i = 0; i < global.airports_counter; i++)
    {
        if (!strcmp(global.airports[i].id, arguments[1]))
        {
            printf("duplicate airport\n");
            return;
        }
    }

    /* If no exception was triggered, create the airport */
    strcpy(global.airports[global.airports_counter].id, arguments[1]);
    strcpy(global.airports[global.airports_counter].country, arguments[2]);
    strcpy(global.airports[global.airports_counter].city, city);
    global.airports_counter++;
    printf("airport %s\n", arguments[1]);
    return;
}

void list_airport(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    if (arguments[1][0] == 0 && global.airports_counter > 0)
    {
        sort_all_airports(arguments);
        return;
    } else {
        print_airport_list(arguments);
        return;
    }
}

void sort_all_airports(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    /*  
        Its supposedly quicker to have these sort of values in register
        instead of memory 
    */
    register int c = global.airports_counter;

    /*  
        We first create an array of pointers to airport codes.
        ISO C90 standard forbids variable lenght arrays (unlike
        C99 for example). Therefore we can't do something like:

            char * airport_codes_ptr_arr[global->airports_counter];

        We have to resort to statically sized buffers.
    */
    airport * airports_ptr_arr[MAX_AIRPORTS];
    for (a = 0; a < c; a++)
    {
        airports_ptr_arr[a] = &(global.airports[a]);
    }

    /*  We sort the array with our quick-sort implementation and with a custom
        comparison function 
    */
    sort_array(airports_ptr_arr, sizeof(airport*), c, flights_code_compare);

    for (a = 0; a < c; a++)
    {
        printf("%s %s %s %d\n", airports_ptr_arr[a]->id, airports_ptr_arr[a]->city, airports_ptr_arr[a]->country, airports_ptr_arr[a]->flights_departing_from_here);
    }
    return;
}

void print_airport_list(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    airport * current_airport;
    for (i = 1; arguments[i] != NULL; i++)
    {
        if(error_helper_noSuchAirportID(arguments[i])) {
            /* We receive the current airport's pointer from the auxiliary function */
            current_airport = helper_find_airport(arguments[i]);
            printf("%s %s %s %d\n", current_airport->id, current_airport->city, current_airport->country, current_airport->flights_departing_from_here);
        }
    }
}

void add_or_list_flights(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    if (arguments[1][0] == 0)
    {
        list_flights(arguments);
        return;
    } else {
        add_flight(arguments);
        return;
    }
}

void list_flights(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    /* We declare the buffers */
    char human_date[11];
    char human_time[6];

    for (a = 0; a < global.flights_counter; a++)
    {
        date_to_human(&human_date, &global.flights[a].departure_date);
        time_to_human(&human_time, &global.flights[a].departure_time);
        printf("%s %s %s %s %s\n", global.flights[a].flight_code, global.flights[a].departure_airport->id, global.flights[a].arrival_airport->id, human_date, human_time);
        memset(&human_date[0], 0, sizeof(human_date));
        memset(&human_time[0], 0, sizeof(human_time));
    }
    return;
}

void add_flight(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    airport *departure_airport_ptr;
    airport *arrival_airport_ptr;
    date converted_departure_date;
    time converted_departure_time, converted_flight_duration;
    int num_passengers;
    register int c = global.flights_counter;

    /*  
        flight_code = arguments[1]
        departure_id = arguments[2]
        arrival_id = arguments[3]
    */

    /* We check for errors that don't require calculations first */
    if(!check_flight_code(arguments[1])){
        return;
    }

    human_to_date(arguments[4], &converted_departure_date);

    if(!check_flight_exists(arguments[1], converted_departure_date)){
        return;
    }
    
    human_to_time(arguments[5], &converted_departure_time);
    human_to_time(arguments[6], &converted_flight_duration);

    if(!too_many_flights()){
        return;
    }
    if (!validate_date(converted_departure_date))
    {
        return;
    }
    if ((converted_flight_duration.hour * 60) + (converted_flight_duration.min) > 720) /* 12 * 60 = 720 minutes */
    {
        printf("invalid duration\n");
        return;
    }
    num_passengers = atoi(arguments[7]);
    if (num_passengers < 10 || num_passengers > 100)
    {
        printf("invalid capacity\n");
        return;
    }

    departure_airport_ptr = helper_find_airport(arguments[2]);
    arrival_airport_ptr = helper_find_airport(arguments[3]);

    departure_airport_ptr->flights_departing_from_here += 1;
    strcpy(global.flights[c].flight_code, arguments[1]);
    global.flights[c].departure_airport = departure_airport_ptr;
    global.flights[c].arrival_airport = arrival_airport_ptr;
    global.flights[c].departure_date = converted_departure_date;
    global.flights[c].departure_time = converted_departure_time;
    global.flights[c].duration = converted_flight_duration;
    global.flights[c].max_passengers = num_passengers;
    global.flights_counter++;
    return;
}

void departure_flights(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    /*  We create an array of pointers to the flights
        in a specific airport.
        As explained previously, C90 does not allow VLA,
        so we must define the array size as the max possible
        number of flights.
    */
    flight *flights_ptr_arr[MAX_FLIGHTS] = {0};
    char buffer_departure_date[11];
    char buffer_departure_time[6];
    int number_of_flights = 0;

    if(!error_helper_noSuchAirportID(arguments[1])){
        return;
    }

    for (a = 0; a < global.flights_counter; a++)
    {
        /* Pointer comparison would be more expensive than plain char comparison here */
        if(!strcmp(global.flights[a].departure_airport->id, arguments[1])){
            flights_ptr_arr[number_of_flights] = &global.flights[a];
            number_of_flights++;
        }
    }
    
    if (number_of_flights)
    {
        /* sort_array(flights_ptr_arr, sizeof(flight *), number_of_flights, flights_departure_datetime_comparison); */
    }
    return;
}

int too_many_flights(){
    if (global.flights_counter + 1 > MAX_FLIGHTS)
    {
        printf("too many flights\n");
        return 0;
    }
    return 1;
}

int check_flight_exists(char flight_code[FLIGHT_CODE_CHARS], date departure_date){
    for (i = 0; i < global.flights_counter; i++) {
        if (!strcmp(global.flights[i].flight_code, flight_code))
        {   
            if (check_same_date(departure_date, global.flights[i].departure_date))
            {
                printf("flight already exists\n");
                return 0;
            }
        }
        }
    return 1;
}

int check_flight_code(char flight_code[FLIGHT_CODE_CHARS]){
    /*  Returns 1 if valid code. 
        Returns 0 and prints error otherwise.
    */
    char tempNum[6];
    if (islower(flight_code[0]) == 0 && islower(flight_code[1]) == 0)
    {
        memcpy(&tempNum[0], &flight_code[2], 5);
        if (atoi(tempNum) > MIN_FLIGHT_CODE_NUM && atoi(tempNum) < MAX_FLIGHT_CODE_NUM)
        {
            return 1;
        }
    }
    printf("invalid flight code\n");
    return 0; 
}

int error_helper_noSuchAirportID(char airport_id[IDENTIFIER_CHARS]) {
    /*  Returns 1 if the airport exists 
        Returns 0 and prints error otherwise.
    */
    if (helper_find_airport(airport_id) == NULL)
    {
        printf("%s: no such airport ID\n", airport_id);
        return 0;
    }
    return 1;
}

airport *helper_find_airport(char airport_id[IDENTIFIER_CHARS])
{
    /*
        Auxiliary function that searches for a specific
        airport and returns its information a pointer to an
        array with a copy of it.
    */
    int i;
    for (i = 0; i < global.airports_counter; i++)
    {
        if (!strncmp(global.airports[i].id, airport_id, IDENTIFIER_CHARS))
        {
            return &global.airports[i];
        }
    }
    return NULL;
}

/* --------------- SORT --------------------- */

void sort_array(void *array, unsigned int size, unsigned  int element_count, long (*compare)(const void *, const void *))
{
    unsigned int left_size = element_count / 2;
    unsigned int right_size = element_count - left_size;
    if(element_count == 1)
    {
        return;
    }
    sort_array(array, size, left_size, compare);
    sort_array((char *)array + (left_size * size), size, right_size, compare);

    merge(array, size, left_size, right_size, compare);
    
}

void merge(void *array, unsigned int size, unsigned int left_size, unsigned  int right_size,  long (*compare)(const void *, const void *)){
    /* Merges two sorted arrays into one sorted array */
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    void *left_array = malloc(size * left_size);
    void *right_array = malloc(size * right_size);
    void *merged_array = malloc(size * (left_size + right_size));

    memcpy(left_array, array, size * left_size);
    memcpy(right_array, (char*) array + (size * left_size), size * right_size);

    while (i < left_size && j < right_size)
    {
        if (compare((char*)left_array + (i * size), (char*)right_array + (j * size)) < 0)
        {
            memcpy((char*) merged_array + (k * size), (char*) left_array + (i * size), size);
            i++;
        } else {
            memcpy((char*) merged_array + (k * size), (char*) right_array + (j * size), size);
            j++;
        }
        k++;
    }

    while (i < left_size)
    {
        memcpy((char*)merged_array + (k * size), (char*)left_array + (i * size), size);
        i++;
        k++;
    }

    while (j < right_size)
    {
        memcpy((char*)merged_array + (k * size), (char*)right_array + (j * size), size);
        j++;
        k++;
    }

    memcpy(array, (char*)merged_array, size * (left_size + right_size));
    free(left_array);
    free(right_array);
    free(merged_array);
}

long flights_code_compare(const void * airport1, const void * airport2){
    const airport * casted_airport1 = * (airport **)airport1;
    const airport * casted_airport2 = * (airport **)airport2;
    return strncmp(casted_airport1->id, casted_airport2->id, IDENTIFIER_CHARS);
}

/* -------------------------------------------------- */

/* --------------- UTILS --------------------- */

int check_same_date(date d1, date d2) {
    /*  Returns 0 if same date, 1 if not.
        (mainly because normally being the same
        date is bad)
    */
    if (d1.day == d2.day && d1.month == d2.month && d1.year == d2.year)
    {
        return 0;
    }
    return 1;
}

void date_to_human(char *buffer, date *date_strct)
{
    sprintf(buffer, "%02d-%02d-%02d", date_strct->day, date_strct->month, date_strct->year);
}

void time_to_human(char *buffer, time *time_strct)
{
    sprintf(buffer, "%02d:%02d", time_strct->hour, time_strct->min);
}

void human_to_date(char *date_human, date *date_buffer)
{
    /*
        From the man page:
        "sscanf() reads its input from the character string pointed to by str"

        Therefor, we can use the same formatting options we would with
        scanf() and dump the result directly into a buffer.

    */
    sscanf(date_human, "%d-%d-%d", &date_buffer->day, &date_buffer->month, &date_buffer->year);
}

void human_to_time(char *time_human, time *time_buffer)
{
    sscanf(time_human, "%d:%d", &time_buffer->hour, &time_buffer->min);
}

int helper_get_datetime(date d, time t)
{
    return ((d.year * 100000000) + (d.month * 1000000) + (d.day * 10000) + (t.hour * 100) + t.min);
}

int validate_date(date new_date) {
    /*  Helper function that guarantees that the date isn't in the past, and isn't more
        than one year in the future.
        Returns 1 if valid, 0 if invalid 
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
        epoch_time += 744; 
    }
    for (a = EPOCH_DAY; a < day; a++)
    {
        epoch_time += 24;
    }
    
    return epoch_time;
}

/* -------------------------------------------------- */