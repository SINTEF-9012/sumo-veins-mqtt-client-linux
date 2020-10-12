//
// Created by root on 4/5/18.
//


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ADKHOST_CONVERSIONS_H
#define ADKHOST_CONVERSIONS_H


int stringToInt(char *string);
void string_copy_local(char *from, char *to);
int is_int(char term[]);
int number_of_digits(int num);
double string_to_double(char *string);

#endif //ADKHOST_CONVERSIONS_H


#ifdef __cplusplus
}
#endif
