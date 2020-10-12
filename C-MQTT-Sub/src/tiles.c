//
// Created by root on 29.08.19.
//

#include <logger.h>
#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <helpers/string_utils.h>

#include "tiles.h"

char *get_tile_based_topic(int sub_zLevel, int main_zLevel, char *tile)
{
    if(sub_zLevel > main_zLevel) {
        print_console(LOG_TYPE_ERROR, "Subscribing zoom level(%d) cannot be greater tham main zoom level(%d)\n",sub_zLevel,main_zLevel);
        return NULL;
    }

    if(tile == NULL){
        return NULL;
    }

    char *prefix = "downstream/quad/";
    char *messagetype = "json";
    size_t length = strlen(prefix) + (main_zLevel*2) + 4 + strlen(messagetype);
    char *topic = malloc(length + 1);
    strcpy(topic, prefix);

    for(int i=0; i<sub_zLevel; i++) {
        char a[1];
        sprintf(a,"%c",tile[i]);
        strcat(topic,a);
        strcat(topic, "/");
        memset(a,0,1);
    }

    for(int i=sub_zLevel; i<main_zLevel; i++) {
        strcat(topic,"+/");
    }

    strcat(topic,"+/"); //message type
    strcat(topic,"+/"); //message id
    strcat(topic,messagetype); //message format

    topic[length] = 0;
    return topic;
}

char *get_tile_for_zoom_level(char *tile, int zoom_level)
{
    char *new_tile = malloc((size_t)zoom_level+1);
    for(int i=0; i<zoom_level; i++) {
        new_tile[i] = tile[i];
    }
    new_tile[zoom_level]='\0';
    return new_tile;
}

char *get_right(char *quad)
{
    char end = quad[strlen(quad) - 1];

    char *final_quad = init_new_string();
    if(end == '0') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '1';
        return final_quad;
    }
    else if (end == '2') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '3';
        return final_quad;
    }
    else if  (end == '1') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"0");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_right(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "0");
        return final_quad;
    }
    else if  (end == '3') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"2");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_right(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "2");
        return final_quad;
    }
    else {
        print_console(LOG_TYPE_ERROR,"Last char of quad is invalid %c\n",end);
        return final_quad;
    }
}

char *get_left(char *quad)
{
    char end = quad[strlen(quad) - 1];

    char *final_quad = init_new_string();
    if(end == '1') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '0';
        return final_quad;
    }
    else if (end == '3') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '2';
        return final_quad;
    }
    else if  (end == '0') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"1");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_left(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "1");
        return final_quad;
    }
    else if  (end == '2') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"3");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_left(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "3");
        return final_quad;
    }
    else {
        print_console(LOG_TYPE_ERROR,"Last char of quad is invalid %c\n",end);
        return final_quad;
    }
}

char *get_top(char *quad)
{
    char end = quad[strlen(quad) - 1];

    char *final_quad = init_new_string();
    if(end == '2') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '0';
        return final_quad;
    }
    else if (end == '3') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '1';
        return final_quad;
    }
    else if  (end == '0') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"2");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_top(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "2");
        return final_quad;
    }
    else if  (end == '1') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"3");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_top(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "3");
        return final_quad;
    }
    else {
        print_console(LOG_TYPE_ERROR,"Last char of quad is invalid %c\n",end);
        return final_quad;
    }
}

char *get_bottom(char *quad)
{
    char end = quad[strlen(quad) - 1];

    char *final_quad = init_new_string();
    if(end == '0') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '2';
        return final_quad;
    }
    else if (end == '1') {
        final_quad = concatenate_string(final_quad,quad);
        final_quad[strlen(final_quad)-1] = '3';
        return final_quad;
    }
    else if  (end == '2') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"0");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_bottom(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "0");
        return final_quad;
    }
    else if  (end == '3') {
        if(strlen(quad) == 1) {
            concatenate_string(final_quad,"1");
            return final_quad;
        }
        char *new_quad = malloc(strlen(quad));
        for(int i=0; i<strlen(quad)-1; i++){
            new_quad[i]=quad[i];
        }
        new_quad[strlen(quad)-1] = 0;
        char *newSub = get_bottom(new_quad);
        free(new_quad);
        final_quad = concatenate_string(final_quad, newSub);
        free(newSub);
        final_quad = concatenate_string(final_quad, "1");
        return final_quad;
    }
    else {
        print_console(LOG_TYPE_ERROR,"Last char of quad is invalid %c\n",end);
        return final_quad;
    }
}

struct tiles_t create_nine_tiles(char *center)
{
    struct tiles_t tiles = DEFAULT_SUBSCRIBED_TILES;
    tiles.center = center;
    tiles.top = get_top(center);
    tiles.top_right = get_right(tiles.top);
    tiles.right = get_right(center);
    tiles.bottom_right = get_bottom(tiles.right);
    tiles.bottom = get_bottom(center);
    tiles.bottom_left = get_left(tiles.bottom);
    tiles.left = get_left(center);
    tiles.top_left = get_top(tiles.left);

    return tiles;
}

void destroy_tiles(struct tiles_t tiles)
{
    free(tiles.center);
    free(tiles.top);
    free(tiles.top_right);
    free(tiles.right);
    free(tiles.bottom_right);
    free(tiles.bottom);
    free(tiles.bottom_left);
    free(tiles.left);
    free(tiles.top_left);
}


void print_tiles(struct tiles_t tiles)
{
    print_console(LOG_TYPE_DEBUG,"center \t\t= %s\n", tiles.center);
    print_console(LOG_TYPE_DEBUG,"top \t\t= %s\n", tiles.top);
    print_console(LOG_TYPE_DEBUG,"top right \t= %s\n", tiles.top_right);
    print_console(LOG_TYPE_DEBUG,"right \t\t= %s\n", tiles.right);
    print_console(LOG_TYPE_DEBUG,"bottom right= %s\n", tiles.bottom_right);
    print_console(LOG_TYPE_DEBUG,"bottom \t\t= %s\n", tiles.bottom);
    print_console(LOG_TYPE_DEBUG,"bottom left = %s\n", tiles.bottom_left);
    print_console(LOG_TYPE_DEBUG,"left \t\t= %s\n", tiles.left);
    print_console(LOG_TYPE_DEBUG,"top left \t= %s\n", tiles.top_left);
}