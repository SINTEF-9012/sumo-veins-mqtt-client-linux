//
// Created by root on 06.09.19.
//


#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C"
{
#endif


#include "veins/modules/application/traci/string_utils.h"
#include "veins/modules/application/traci/tile.h"
#include "veins/modules/application/traci/logger.h"


char *get_station_id();

char *get_tile_based_topic4pub(int zoom_level, char *tile)
{
    size_t tileCount = strlen(tile);
    if(zoom_level > tileCount) {
        print_console(LOG_TYPE_ERROR, "Publishing zoom level(%d) cannot be greater than number of digits in tile(%d)\n",zoom_level,tileCount);
        return NULL;
    }

    if(tile == NULL){
        return NULL;
    }

    char *prefix = "upstream/quad";
    char *messagetype = "json";
    char *format = "array";
    char *station_id = get_station_id();

    char *quad = init_new_string();
    for(int i=0; i<zoom_level; i++) {
        char *str = (char *)malloc(3);
        str[0] = tile[i];
        str[1] = '/';
        str[2] = '\0';
        quad = concatenate_string(quad,str);
        free(str);
    }

    char *topic = init_new_string();
    topic = concatenate_string(topic, prefix);
    topic = concatenate_string(topic,"/");
    topic = concatenate_string(topic,quad);
    topic = concatenate_string(topic,format);
    topic = concatenate_string(topic,"/");
    topic = concatenate_string(topic,station_id);
    topic = concatenate_string(topic,"/");
    topic = concatenate_string(topic,messagetype);

    free(station_id);
    free(quad);


    return topic;
}

char *get_station_id() {
    return NULL;
}


char *get_tile_for_zoom_level4pub(char *tile, int zoom_level)
{
    char *new_tile = (char *) malloc((size_t)zoom_level+1);
    for(int i=0; i<zoom_level; i++) {
        new_tile[i] = tile[i];
    }
    new_tile[zoom_level]='\0';
    return new_tile;
}



#ifdef __cplusplus
}
#endif
