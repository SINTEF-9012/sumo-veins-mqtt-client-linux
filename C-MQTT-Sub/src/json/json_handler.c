//
// Created by root on 07.08.18.
//

#include <jansson.h>
#include <memory.h>
//#include <db/db_access.h>
#include "json_handler.h"

json_t *load_json(const char *text);

char *get_messagetype(char *message)
{
    json_t *root = load_json(message);

    if(root) {

        const char *key;
        json_t *value;


        json_object_foreach(root, key, value) {

            if(strcmp(key,MESSAGETYPE_CAM) == 0) {
                char *messagetype = malloc(sizeof(*messagetype) * 5);
                strcpy(messagetype, MESSAGETYPE_CAM);
                messagetype[4] = '\0';

                json_decref(root);
                return messagetype;

            } else if(strcmp(key,MESSAGETYPE_DENM) == 0) {
                char *messagetype = malloc(sizeof(*messagetype) * 6);
                strcpy(messagetype, MESSAGETYPE_DENM);
                messagetype[5] = '\0';

                json_decref(root);
                return messagetype;

            } else if(strcmp(key,MESSAGETYPE_SAM) == 0) {
                char *messagetype = malloc(sizeof(*messagetype) * 5);
                strcpy(messagetype, MESSAGETYPE_SAM);
                messagetype[4] = '\0';

                json_decref(root);
                return messagetype;

            } else if(strcmp(key,MESSAGETYPE_IVI) == 0) {
                char *messagetype = malloc(sizeof(*messagetype) * 5);
                strcpy(messagetype, MESSAGETYPE_IVI);
                messagetype[4] = '\0';

                json_decref(root);
                return messagetype;

            } else if(strcmp(key,MESSAGETYPE_UNKNOWN) == 0) {
                char *messagetype = malloc(sizeof(*messagetype) * 5);
                strcpy(messagetype, MESSAGETYPE_UNKNOWN);
                messagetype[4] = '\0';

                json_decref(root);
                return messagetype;

            }
        }

        json_decref(root);
    }
    char *messageType = malloc(strlen(UNDEFINED)+1);
    strcpy(messageType,UNDEFINED);
    messageType[strlen(UNDEFINED)]=0;
    return messageType;
}

char *get_stationid(char *message)
{
    json_t *root = load_json(message);
    char *stationid = malloc(20);

    if(root) {
        if(json_typeof(root) == JSON_OBJECT) {
            json_t *jheader = json_object_get(root,"header");
            if (jheader == NULL) {
                return UNDEFINED;
            }
            if(json_typeof(jheader) == JSON_OBJECT) {
                json_t *jstid = json_object_get(jheader,"stationID");
                sprintf(stationid,"%" JSON_INTEGER_FORMAT "",json_integer_value(jstid));
                int len = (int)strlen(stationid);
                stationid[len] = '\0';

                json_decref(root);
                return stationid;
            }
        }
    }

    json_decref(root);
    return UNDEFINED;
}

char *construct_message(char *org_message)
{
    char *s = NULL;

    json_t *msg = load_json(org_message);

    json_t *root = json_object();
    //json_object_set_new(root, ATTR_MESSAGES_server_synced, json_integer(1));
    //json_object_set_new(root, ATTR_MESSAGES_radio_synced, json_integer(0));
    //json_object_set_new(root, ATTR_MESSAGES_message, msg);

    s = json_dumps(root, 0);
    json_decref(msg);
    json_decref(root);
    return s;
}


json_t *load_json(const char *text) {
    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

    if (root) {
        return root;
    } else {
        fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
        json_decref(root);
        return (json_t *)0;
    }
}