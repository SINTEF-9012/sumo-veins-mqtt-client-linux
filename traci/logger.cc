//
// Created by root on 4/3/18.
//


#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <sys/stat.h>
//#include <log4c.h>
#include <pthread.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C"
{
#endif


#include "veins/modules/application/traci/config.h"
#include "veins/modules/application/traci/logger.h"


pthread_mutex_t mutex_logger = PTHREAD_MUTEX_INITIALIZER;
void create_dev_dirs();
void create_qfree_dirs();

int logger_initialized;

int init_logger()
{
    logger_initialized = 0;

    char *env = get_env();
    print_console(LOG_TYPE_DEBUG,"Running env: %s\n",env);
    if(strcmp(env,ENVIRONMENT_QFREE)==0) {
        create_qfree_dirs();
    } else if(strcmp(env,ENVIRONMENT_DEV)==0) {
        create_dev_dirs();
    }


    //if (log4c_init()){
    //    print_console(LOG_TYPE_ERROR,"log4c_init() failed\n");
    //    logger_initialized = 0;
    //    return -1;
    //}

    logger_initialized = 1;
    return 0;
}

void create_dev_dirs()
{
    struct stat st = {0};

    if (stat(LOG_DIR_DEV, &st) == -1) {
        mkdir(LOG_DIR_DEV, 0755);
    }

    if (stat(APP_DIR_DEV, &st) == -1) {
        mkdir(APP_DIR_DEV, 0755);
    }

    if (stat(LOG_DIR_MESSAGES_READ_DEV, &st) == -1) {
        mkdir(LOG_DIR_MESSAGES_READ_DEV, 0755);
    }

    if (stat(LOG_DIR_MESSAGES_WRITE_DEV, &st) == -1) {
        mkdir(LOG_DIR_MESSAGES_WRITE_DEV, 0755);
    }

    if (stat(LOG_DIR_ERROR_DEV, &st) == -1) {
        mkdir(LOG_DIR_ERROR_DEV, 0755);
    }

    if (stat(LOG_DIR_QUERY_DEV, &st) == -1) {
        mkdir(LOG_DIR_QUERY_DEV, 0755);
    }

    if (stat(LOG_DIR_DEBUG_DEV, &st) == -1) {
        mkdir(LOG_DIR_DEBUG_DEV, 0755);
    }

    if (stat(LOG_DIR_ALL_DEV, &st) == -1) {
        mkdir(LOG_DIR_ALL_DEV, 0755);
    }
}

void create_qfree_dirs()
{
    struct stat st = {0};

    if (stat(LOG_DIR_QFREE, &st) == -1) {
        mkdir(LOG_DIR_QFREE, 0755);
    }

    if (stat(APP_DIR_QFREE, &st) == -1) {
        mkdir(APP_DIR_QFREE, 0755);
    }

    if (stat(LOG_DIR_MESSAGES_READ_QFREE, &st) == -1) {
        mkdir(LOG_DIR_MESSAGES_READ_QFREE, 0755);
    }

    if (stat(LOG_DIR_MESSAGES_WRITE_QFREE, &st) == -1) {
        mkdir(LOG_DIR_MESSAGES_WRITE_QFREE, 0755);
    }

    if (stat(LOG_DIR_ERROR_QFREE, &st) == -1) {
        mkdir(LOG_DIR_ERROR_QFREE, 0755);
    }

    if (stat(LOG_DIR_QUERY_QFREE, &st) == -1) {
        mkdir(LOG_DIR_QUERY_QFREE, 0755);
    }

    if (stat(LOG_DIR_DEBUG_QFREE, &st) == -1) {
        mkdir(LOG_DIR_DEBUG_QFREE, 0755);
    }

    if (stat(LOG_DIR_ALL_QFREE, &st) == -1) {
        mkdir(LOG_DIR_ALL_QFREE, 0755);
    }
}

int deinit_logger()
{
    print_console(LOG_TYPE_DEBUG, "Deinitializing logger\n");
    //if ( log4c_fini()){
        print_console(LOG_TYPE_ERROR,"log4c_fini() failed");
    //}
    logger_initialized = 0;
    return 0;
}


void print_console(log_type_t level, const char *__restrict __format, ...)
{
    pthread_mutex_lock(&mutex_logger);
    char *message;

    va_list arg;
    va_start(arg, __format);
    size_t size = (size_t)vsnprintf(NULL,0,__format,arg)+1;
    va_end(arg);

    message = (char *)malloc(size+1);

    va_start(arg, __format);
    vsnprintf(message,size,__format,arg);
    va_end(arg);

    message[strlen(message)] = '\0';

    /*
    if (level <= get_log_level()) {
        if(level == LOG_TYPE_ERROR){
            log4c_category_t* ecat = NULL;
            ecat = log4c_category_get(LOG_CATEGORY_ERROR);
            log4c_category_log(ecat, LOG4C_PRIORITY_DEBUG, message);
        } else if (level == LOG_TYPE_MESSAGE_READ) {
            log4c_category_t* icat = NULL;
            icat = log4c_category_get(LOG_CATEGORY_MESSAGES_READ);
            log4c_category_log(icat, LOG4C_PRIORITY_DEBUG, message);

        } else if (level == LOG_TYPE_MESSAGE_WRITE) {
            log4c_category_t* icat = NULL;
            icat = log4c_category_get(LOG_CATEGORY_MESSAGES_WRITE);
            log4c_category_log(icat, LOG4C_PRIORITY_DEBUG, message);

        } else if (level == LOG_TYPE_DEBUG) {
            log4c_category_t* icat = NULL;
            icat = log4c_category_get(LOG_CATEGORY_DEBUG);
            log4c_category_log(icat, LOG4C_PRIORITY_DEBUG, message);

        } else if (level == LOG_TYPE_QUERY) {
            log4c_category_t* icat = NULL;
            icat = log4c_category_get(LOG_CATEGORY_QUERIES);
            log4c_category_log(icat, LOG4C_PRIORITY_DEBUG, message);
        }
    }
*/


    if (level <= get_print_level()) {
        if(level == LOG_TYPE_ERROR){
            fprintf (stderr, "%s", message);
        } else {
            fprintf (stdout, "%s", message);
        }
    }

    free(message);
    pthread_mutex_unlock(&mutex_logger);
}



#ifdef __cplusplus
}
#endif
