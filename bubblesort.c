/* ist1102416 - Tomás Simões */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj1.h"

void sort_array(void *arr, unsigned int type_size,unsigned int number_elements, long (*comparison)(const void *, const void *))
{
    char *arr_bytes = arr;
    for (i = 0; i < number_elements - 1; i++)
    {
        for (j = 0; j < number_elements - i - 1; j++)
        {
            if (comparison(arr_bytes + j * type_size, arr_bytes + (j + 1) * type_size) > 0)
            {
                void *temp = malloc(type_size);
                memcpy(temp, arr_bytes + j * type_size, type_size);
                memcpy(arr_bytes + j * type_size, arr_bytes + (j + 1) * type_size, type_size);
                memcpy(arr_bytes + (j + 1) * type_size, temp, type_size);
                free(temp);
            }
        }
    }
}

long airports_id_compare(const void * airport1, const void * airport2) {
    /* Compares two airports by their ID, for use in the sorter. */
    const airport * casted_airport1 = * (airport **)airport1;
    const airport * casted_airport2 = * (airport **)airport2;
    return strncmp(casted_airport1->id, casted_airport2->id, IDENTIFIER_CHARS);
}

long flights_departure_datetime_compare(const void *flight1, const void *flight2) {   
    /* Compares two airports by their ID, for use in the sorter. */
    const flight *casted_flight1 = *(flight **)flight1;
    const flight *casted_flight2 = *(flight **)flight2;
    /*  Returns >0 if the datetime of flight1 is larger than flight2 */
    return helper_get_datetime(casted_flight1->departure_date, casted_flight1->departure_time) - helper_get_datetime(casted_flight2->departure_date, casted_flight2->departure_time);
}

long flights_arrival_datetime_compare(const void *flight1, const void *flight2)
{
    /* Compares two airports by their ID, for use in the sorter. */
    const flight *casted_flight1 = *(flight **)flight1;
    const flight *casted_flight2 = *(flight **)flight2;
    return helper_get_datetime(casted_flight1->arrival_date, casted_flight1->arrival_time) - helper_get_datetime(casted_flight2->arrival_date, casted_flight2->arrival_time);  
}