//
// Created by root on 06.09.19.
//


#ifdef __cplusplus
extern "C"
{
#endif


#ifndef C_MQTT_PUB_TILE_H
#define C_MQTT_PUB_TILE_H


char *get_tile_based_topic4pub(int zoom_level, char *tile);
char *get_tile_for_zoom_level4pub(char *tile, int zoom_level);


#endif //C_MQTT_PUB_TILE_H


#ifdef __cplusplus
}
#endif
