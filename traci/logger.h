//
// Created by root on 4/3/18.
//


#ifdef __cplusplus
extern "C"
{
#endif


#ifndef ADKHOST_LOGGER_H
#define ADKHOST_LOGGER_H

typedef enum {
    LOG_TYPE_NONE           = 0,
    LOG_TYPE_ERROR          = 1,
    LOG_TYPE_MESSAGE_READ   = 2,
    LOG_TYPE_MESSAGE_WRITE  = 3,
    LOG_TYPE_DEBUG          = 4,
    LOG_TYPE_QUERY          = 5,

} log_type_t;

int init_logger();
void print_console(log_type_t level, const char *__restrict __format, ...);
int deinit_logger();

#endif //ADKHOST_LOGGER_H



#ifdef __cplusplus
}
#endif
