//
// Created by root on 4/5/18.
//


#include <string.h>
#include <stdio.h>
#include <zconf.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C"
{
#endif

#include "veins/modules/application/traci/conversions.h"


pthread_mutex_t mutex_sti = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_stodouble = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_scopy = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_isint = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ndigits = PTHREAD_MUTEX_INITIALIZER;

int stringToInt(char *string)
{
    pthread_mutex_lock(&mutex_sti);
    int i;
    int dec=0;
    size_t len = strlen(string);
    for(i=0; i<len; i++){
        dec = dec * 10 + ( string[i] - '0' );
    }
    pthread_mutex_unlock(&mutex_sti);
    return dec;
}


double string_to_double(char *string) {

    pthread_mutex_lock(&mutex_stodouble);
    char *ptr;
    double ret;

    ret = strtod(string, &ptr);

    pthread_mutex_unlock(&mutex_stodouble);
    return ret;
}

void string_copy_local(char *from, char *to)
{
    pthread_mutex_lock(&mutex_scopy);
    while ((*to++ = *from++) != '\0')
    {
        ;
    }
    pthread_mutex_unlock(&mutex_scopy);
}

int is_int(char term[])
{
    pthread_mutex_lock(&mutex_isint);
    int isInt = 1;
    for(int i=0; i<strlen(term); i++) {
        if (isdigit(term[i])){
            continue;
        } else {
            isInt = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_isint);
    return isInt;
}

int number_of_digits(int num)
{
    pthread_mutex_lock(&mutex_ndigits);
    int count = 0;

    while (num != 0) {
        num/=10;
        ++count;
    }

    pthread_mutex_unlock(&mutex_ndigits);
    return  count;
}



#ifdef __cplusplus
}
#endif
