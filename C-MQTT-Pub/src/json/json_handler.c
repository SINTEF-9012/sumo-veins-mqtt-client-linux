//
// Created by root on 11/7/18.
//

#include <jansson.h>
#include <memory.h>
#include <conversions.h>
#include <C-MQTT-Pub/src/config.h>
#include <C-MQTT-Pub/src/config.h>
#include <C-MQTT-Pub/src/json/json_handler.h>

json_t *load_json(const char *text);


char *get_bson_id(char *full_message)
{
    json_t *root = load_json(full_message);

    if (root) {

        if(json_typeof(root) == JSON_OBJECT) {

            json_t *jid = json_object_get(root,"_id");
            if(jid == NULL || json_typeof(jid) != JSON_OBJECT) {
                return NULL;
            }

            if(json_typeof(jid) == JSON_OBJECT) {

                json_t *joid = json_object_get(jid, "$oid");
                if(joid == NULL || json_typeof(joid) != JSON_STRING) {
                    return NULL;
                }

                const char *id = json_string_value(joid);
                char *rid = malloc(strlen(id) + 1);
                string_copy((char *)id,rid);

                json_decref(root);
                return rid;
            }
        }
    }

    json_decref(root);
    return NULL;
}


char *get_message(char *full_message)
{
    json_t *root = load_json(full_message);

    if (root) {

        if(json_typeof(root) == JSON_OBJECT) {

            json_t *jmessage = json_object_get(root,"message");
            if(jmessage == NULL || json_typeof(jmessage) != JSON_OBJECT) {
                return NULL;
            }

            const char *message = json_dumps(jmessage,0);

            json_decref(root);
            return (char *)message;
        }
    }

    json_decref(root);
    return NULL;
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