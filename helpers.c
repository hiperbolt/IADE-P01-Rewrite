/* ist1102416 - Tomás Simões */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj1.h"

/* 
    This file contains helper functions and
    subroutines to the main functions.
                                            */

int too_many_flights(){
    /* 
        Catches and throws too many flights err 
        Returns 0 and throws err if too many flights,
        returns 1 otherwise.
    */
    if (global.flights_counter + 1 > MAX_FLIGHTS)
    {
        printf("too many flights\n");
        return 0;
    }
    return 1;
}

int check_flight_exists(char flight_code[FLIGHT_CODE_CHARS], date departure_date){
    /* 
        Checks if a flight exists in the system.
        Returns 0 if it does not exist, 1 otherwise.
    */
    for (a = 0; a < global.flights_counter; a++) {
        if (!strcmp(global.flights[a].flight_code, flight_code))
        {   
            if (!check_same_date(departure_date, global.flights[a].departure_date))
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
    if (islower(flight_code[0]) == 0 && islower(flight_code[1]) == 0 && flight_code[2] != '0')
    {
        memcpy(&tempNum[0], &flight_code[2], 5);
        if (atoi(tempNum) >= MIN_FLIGHT_CODE_NUM && atoi(tempNum) <= MAX_FLIGHT_CODE_NUM)
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

airport *helper_find_airport(char airport_id[IDENTIFIER_CHARS]) {
    /* 
        Returns the airport pointer if it exists.
        Returns NULL otherwise.

        Uses strncmp for efficency and (some) memory safety.
    */
    
    for (a = 0; a < global.airports_counter; a++)
    {
        if (!strncmp(global.airports[a].id, airport_id, IDENTIFIER_CHARS))
        {
            return &global.airports[a];
        }
    }
    return NULL;
}