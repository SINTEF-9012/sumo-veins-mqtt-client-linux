//
// Created by root on 07.08.18.
//

#ifndef C_MQTT_SUB_JSON_HANDLER_H
#define C_MQTT_SUB_JSON_HANDLER_H


#define UNDEFINED "NOT_DEFINED"

#define MESSAGETYPE_CAM         "cam"
#define MESSAGETYPE_DENM        "denm"
#define MESSAGETYPE_SAM         "sam"
#define MESSAGETYPE_IVI         "ivi"
#define MESSAGETYPE_UNKNOWN     "ukn"

char *get_messagetype(char *message);
char *get_stationid(char *message);
char *construct_message(char *org_message);


#endif //C_MQTT_SUB_JSON_HANDLER_H
