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

    fgets(input_array, 1000, stdin);
        
    /*  pch (pointer char) points at the current strtok token
        and is updated until strtok returns a null token */
    pch = strtok(input_array, " \n\t\r");
    for (a = 0; pch != NULL; a++)
    {   
        strcpy(arguments[a], pch);
        if (a >= 2 && input_array[0] == 'a')
        {   
            pch = strtok(NULL, "\n");
        } else {
            pch = strtok(NULL, " \n\t\r");
        }   
    }
    return(handler(input_array[0], arguments));
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
        case 'c':
            arrival_flights(arguments);
            return 1;
        case 't':
            advance_date(arguments);
            return 1;
        default:
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

    for (a = 0; a < global.airports_counter; a++)
    {
        if (!strcmp(global.airports[a].id, arguments[1]))
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

void sort_all_airports() {
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
    sort_array(airports_ptr_arr, sizeof(airport*), c, airports_id_compare);

    for (a = 0; a < c; a++)
    {
        printf("%s %s %s %d\n", airports_ptr_arr[a]->id, airports_ptr_arr[a]->city, airports_ptr_arr[a]->country, airports_ptr_arr[a]->flights_departing_from_here);
    }
    return;
}

void print_airport_list(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    airport * current_airport;
    for (j = 1; arguments[j][0] != 0; j++)
    {
        if (j > MAX_AIRPORTS)
        {
            return;
        }
        if(error_helper_noSuchAirportID(arguments[j])) {
            /* We receive the current airport's pointer from the auxiliary function */
            current_airport = helper_find_airport(arguments[j]);
            printf("%s %s %s %d\n", current_airport->id, current_airport->city, current_airport->country, current_airport->flights_departing_from_here);
        }
    }
    return;
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

void list_flights() {
    /* We declare the buffers */
    char human_date[11];
    char human_time[6];

    for (a = 0; a < global.flights_counter; a++)
    {
        date_to_human(human_date, &global.flights[a].departure_date);
        time_to_human(human_time, &global.flights[a].departure_time);
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
    
    if (!error_helper_noSuchAirportID(arguments[2]) || !error_helper_noSuchAirportID(arguments[3])) {
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
        sort_array(flights_ptr_arr, sizeof(flight *), number_of_flights, flights_departure_datetime_compare);
    }

    for (a = 0; a < number_of_flights; a++)
    {
        date_to_human(buffer_departure_date, &flights_ptr_arr[a]->departure_date);
        time_to_human(buffer_departure_time, &flights_ptr_arr[a]->departure_time);
        printf("%s %s %s %s\n", flights_ptr_arr[a]->flight_code, flights_ptr_arr[a]->arrival_airport->id, buffer_departure_date, buffer_departure_time);
    }
    return;
}

void arrival_flights(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    flight *flights_ptr_arr[MAX_FLIGHTS] = {0};
    char buffer_arrival_date[11];
    char buffer_arrival_time[6];
    int number_of_flights = 0;

    if(!error_helper_noSuchAirportID(arguments[1])){
        return;
    }

    for (a = 0; a < global.flights_counter; a++)
    {
        /*  
            As previously stated, pointer comparison would be increbidly expensive
            compared to plain char comparison in this
            case.
         */
        if(!strcmp(global.flights[a].arrival_airport->id, arguments[1])){
            /*  
                We're going to take advantage of the fact we're alredy looking at each
                airport individually and calculate the arrival date and time.
                This will cost us some time and computational power in this function, however,
                it will also make the sorting itself faster.
            */
            calculate_arrival_datetime(&global.flights[a].arrival_date, &global.flights[a].arrival_time, global.flights[a].departure_date, global.flights[a].departure_time, global.flights[a].duration);
            flights_ptr_arr[number_of_flights] = &global.flights[a];
            number_of_flights++;
        }
    }

    if (number_of_flights)
    {
        sort_array(flights_ptr_arr, sizeof(flight *), number_of_flights, flights_arrival_datetime_compare);
    }

    for (a = 0; a < number_of_flights; a++)
    {
        date_to_human(buffer_arrival_date, &flights_ptr_arr[a]->arrival_date);
        time_to_human(buffer_arrival_time, &flights_ptr_arr[a]->arrival_time);
        printf("%s %s %s %s\n", flights_ptr_arr[a]->flight_code, flights_ptr_arr[a]->departure_airport->id, buffer_arrival_date, buffer_arrival_time);
    }
    return;
}

void advance_date(char (*arguments)[MAX_ARGUMENT_SIZE]) {
    date new_date;
    
    human_to_date(arguments[1], &new_date);
    if (!validate_date(new_date))
    {
        return;
    }

    global.current_date = new_date;
    printf("%s\n", arguments[1]);
    return;
}
