//
// Created by root on 29.08.19.
//

#ifndef C_MQTT_SUB_TILES_H
#define C_MQTT_SUB_TILES_H


struct tiles_t {
    char *center;
    char *top;
    char *top_right;
    char *right;
    char *bottom_right;
    char *bottom;
    char *bottom_left;
    char *left;
    char *top_left;
};

#define DEFAULT_SUBSCRIBED_TILES    { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL }


char *get_tile_based_topic(int sub_zLevel, int main_zLevel, char *tile);
char *get_tile_for_zoom_level(char *tile, int zoom_level);
struct tiles_t create_nine_tiles(char *center);
void destroy_tiles(struct tiles_t tiles);
void print_tiles(struct tiles_t tiles);

#endif //C_MQTT_SUB_TILES_H
