//
// Created by root on 5/21/19.
//

#include <C-MQTT-Pub/src/helpers/string_utils.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

char *concatenate_string(char *original, char *add)
{

    size_t org_size = strlen(original);
    size_t add_size = strlen(add);


    size_t newSize = org_size + add_size + 1;

    original = realloc(original, newSize);

    strcat(original,add);
    original[strlen(original)] = '\0';

    return original;
}

char *init_new_string()
{
    char *string = malloc(1);
    strcpy(string,"");
    return string;
}