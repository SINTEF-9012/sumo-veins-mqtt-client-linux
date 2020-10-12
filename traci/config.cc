//
// Created by root on 6/25/18.
//



#include <libconfig.h>
#include <stdlib.h>
#include <memory.h>


#ifdef __cplusplus
extern "C"
{
#endif



#include "veins/modules/application/traci/logger.h"
#include "veins/modules/application/traci/conversions.h"
#include "veins/modules/application/traci/config.h"


//const char* get_value_for_key(char* key);
const char* get_value_for_key(char* key);
int get_int_value_for_key(char* key);
config_setting_t *get_element_for_key(char* key);

config_t cfg;

char *env;
int log_level;
int print_level;

char *mqtt_server;
char *mqtt_username;
char *mqtt_password;
char *mqtt_clientid;
char *mqtt_topic;
int mqtt_qos;
int mqtt_qos_cam;
int publishing_interval;
int topic_sub_enabled;
int single_tile_sub_enabled;
int single_tile_main_zLevel;
int single_tile_sub_zLevel;
int nine_tiles_sub_enabled;
int nine_tile_main_zLevel;
int nine_tile_sub_zLevel;
int pub_zoom_level;
struct topic_t stopic;

int ssl_enabled;
char *ssl_capath;

void init_config_pub()
{

    char *cqtt_server = (char *) get_value_for_key("mqtt.address");
    mqtt_server = (char *)malloc(strlen(cqtt_server)+1);
    string_copy_local(cqtt_server,mqtt_server);
    config_destroy(&cfg);

    char *cmqtt_username = (char *)get_value_for_key("mqtt.username");
    mqtt_username = (char *)malloc(strlen(cmqtt_username)+1);
    string_copy_local(cmqtt_username,mqtt_username);
    config_destroy(&cfg);

    char *cmqtt_password = (char *)get_value_for_key("mqtt.password");
    mqtt_password = (char *)malloc(strlen(cmqtt_password)+1);
    string_copy_local(cmqtt_password,mqtt_password);
    config_destroy(&cfg);

    char *cmqtt_clientid = (char *)get_value_for_key(MQTT_CLIENT_ID);
    mqtt_clientid = (char *)malloc(strlen(cmqtt_clientid)+1);
    string_copy_local(cmqtt_clientid,mqtt_clientid);
    config_destroy(&cfg);

    char *cmqtt_topic = (char *)get_value_for_key(MQTT_TOPIC);
    mqtt_topic = (char *)malloc(strlen(cmqtt_topic)+1);
    string_copy_local(cmqtt_topic,mqtt_topic);
    config_destroy(&cfg);


    char *strqos = (char *)get_value_for_key(MQTT_QOS);
    char *ptrqos;
    long retqos = strtol(strqos,&ptrqos,10);
    mqtt_qos = (int)retqos;
    config_destroy(&cfg);


    topic_sub_enabled = get_int_value_for_key(TOPIC_SUBSCRIBER_ENABLED);
    config_destroy(&cfg);

    config_setting_t *setting = get_element_for_key(TOPIC_SUBSCRIBER_TOPICS);
    int count = config_setting_length(setting);
    struct topic_t top = DEFAULT_TOPIC_INITIALIZER;
    top.topics = (char **)malloc(sizeof(top.topics)*count);
    for(int i=0; i<count; i++) {
        const char *curtopic = config_setting_get_string_elem(setting,i);
        top.topics[i] = (char *)malloc(strlen(curtopic)+1);
        strcpy(top.topics[i],curtopic);
        top.size++;
    }
    config_destroy(&cfg);
    stopic = top;


    single_tile_sub_enabled = get_int_value_for_key(SINGLE_TILE_SUBSCRIBER_ENABLED);
    config_destroy(&cfg);

    char *str_singlemainzlevel = (char *)get_value_for_key(SINGLE_TILE_MAIN_ZOOMLEVEL);
    char *ptr_singlemainzlevel;
    if(str_singlemainzlevel == NULL) {
        fprintf(stderr,"%s not found\n",SINGLE_TILE_MAIN_ZOOMLEVEL);
    } else {
        long ret_singlemainzlevel = strtol(str_singlemainzlevel,&ptr_singlemainzlevel,10);
        single_tile_main_zLevel = (int)ret_singlemainzlevel;
    }
    config_destroy(&cfg);


    char *str_singlesubzlevel = (char *)get_value_for_key(SINGLE_TILE_SUB_ZOOMLEVEL);
    char *ptr_singlesubzlevel;
    if(str_singlesubzlevel == NULL) {
        fprintf(stderr,"%s not found\n",SINGLE_TILE_SUB_ZOOMLEVEL);
    } else {
        long ret_singlesubzlevel = strtol(str_singlesubzlevel,&ptr_singlesubzlevel,10);
        single_tile_sub_zLevel = (int)ret_singlesubzlevel;
    }
    config_destroy(&cfg);



    nine_tiles_sub_enabled = get_int_value_for_key(NINE_TILE_SUBSCRIBER_ENABLED);
    config_destroy(&cfg);


    char *str_ninemainzlevel = (char *)get_value_for_key(NINE_TILE_MAIN_ZOOMLEVEL);
    char *ptr_ninemainzlevel;
    if(str_ninemainzlevel == NULL) {
        fprintf(stderr,"%s not found\n",NINE_TILE_MAIN_ZOOMLEVEL);
    } else {
        long ret_ninemainzlevel = strtol(str_ninemainzlevel,&ptr_ninemainzlevel,10);
        nine_tile_main_zLevel = (int)ret_ninemainzlevel;
    }
    config_destroy(&cfg);


    char *str_ninesubzlevel = (char *)get_value_for_key(NINE_TILE_SUB_ZOOMLEVEL);
    char *ptr_ninesubzlevel;
    if(str_ninesubzlevel == NULL) {
        fprintf(stderr,"%s not found\n",NINE_TILE_SUB_ZOOMLEVEL);
    } else {
        long ret_ninesubzlevel = strtol(str_ninesubzlevel,&ptr_ninesubzlevel,10);
        nine_tile_sub_zLevel = (int)ret_ninesubzlevel;
    }
    config_destroy(&cfg);


    char *str_pub_zoomLevel = (char *)get_value_for_key(PUBLISHER_ZOOMLEVEL);
    char *ptr_pub_zoomLevel;
    if(str_pub_zoomLevel == NULL) {
        fprintf(stderr,"%s not found\n",PUBLISHER_ZOOMLEVEL);
    } else {
        long ret_pub_zoom_level = strtol(str_pub_zoomLevel,&ptr_pub_zoomLevel,10);
        pub_zoom_level = (int)ret_pub_zoom_level;
    }
    config_destroy(&cfg);



    char *strqoscam = (char *)get_value_for_key(MQTT_QOS_CAM);
    char *ptrqoscam;
    long retqoscam = strtol(strqoscam,&ptrqoscam,10);
    mqtt_qos_cam = (int)retqoscam;
    config_destroy(&cfg);


    char *strinterval = (char *)get_value_for_key(PUB_INTERVAL);
    char *ptrinterval;
    long retinterval = strtol(strinterval,&ptrinterval,10);
    publishing_interval = (int)retinterval;
    config_destroy(&cfg);


    char *envval = (char *)get_value_for_key("env");
    env = (char *)malloc(strlen(envval)+1);
    strcpy(envval,env);
    config_destroy(&cfg);


    char *str = (char *)get_value_for_key("log_level");
    char *ptr;
    long ret = strtol(str,&ptr,10);
    log_level = (int)ret;
    config_destroy(&cfg);

    char *str2 = (char *)get_value_for_key("print_level");
    char *ptr2;
    long ret2 = strtol(str2,&ptr2,10);
    print_level = (int)ret2;
    config_destroy(&cfg);

    char *sslenbl = (char *)get_value_for_key(MQTT_SSL_ENABLED);
    char *sslenbl2;
    long ret5 = strtol(sslenbl,&sslenbl2,10);
    ssl_enabled = (int)ret5;
    config_destroy(&cfg);

    char *sslcapath = (char *)get_value_for_key(MQTT_SSL_CAPATH);
    ssl_capath = (char *)malloc(strlen(sslcapath)+1);
    strcpy(sslcapath,ssl_capath);
    config_destroy(&cfg);

}

void deinit_config()
{
    printf("Cleaning config \n");
    free(env);

    free(mqtt_server);
    free(mqtt_username);
    free(mqtt_password);
    free(mqtt_clientid);
    free(mqtt_topic);
    free(ssl_capath);

    for(int i=0; i<stopic.size; i++) {
        free(stopic.topics[i]);
    }
    free(stopic.topics);
}


char* get_env()
{
    return env;
}

int get_log_level()
{
    return log_level;
}

int get_print_level()
{
    return print_level;
}

char* get_mqtt_server()
{
    return mqtt_server;
}

char* get_mqtt_username()
{
    return mqtt_username;
}

char* get_mqtt_password()
{
    return mqtt_password;
}


char* get_mqtt_client_id_pub()
{
    return mqtt_clientid;
}


char* get_ssl_capath()
{
    return ssl_capath;

}

int get_ssl_enabled()
{
    return ssl_enabled;

}


int get_mqtt_sub_qos()
{
    return mqtt_qos;
}

int get_single_tile_subscriber_enabled()
{
    return single_tile_sub_enabled;
}

int get_single_tile_mainZoom_level()
{
    return single_tile_main_zLevel;
}

int get_single_tile_subZoom_level()
{
    return single_tile_sub_zLevel;
}

int get_topic_subscriber_enabled()
{
    return topic_sub_enabled;
}

int get_nine_tile_subscriber_enabled()
{
    return nine_tiles_sub_enabled;
}

int get_nine_tile_mainZoom_level()
{
    return nine_tile_main_zLevel;
}

int get_nine_tile_subZoom_level()
{
    return nine_tile_sub_zLevel;
}

struct topic_t get_topics()
{
    return stopic;
}

int get_pub_zoom_level()
{
    return pub_zoom_level;
}


int get_mqtt_pub_qos_local(int message_id)
{
    if(message_id == ITS_MESSAGEID_CAM || message_id == ITS_MESSAGEID_SIMPLE_CAM){
        return mqtt_qos_cam;
    }
    return mqtt_qos;
}

int get_publishig_interval()
{
    return publishing_interval;
}



const char* get_value_for_key(char* key)
{
    config_init(&cfg);
    config_setting_t *setting;

    const char *str;

    if(!config_read_file(&cfg, QFREE_CONFIG_LOCATION)) {

        if(!config_read_file(&cfg, "../../config.cfg"))
        {

            printf("%s:%d - %s\n", config_error_file(&cfg),
                   config_error_line(&cfg), config_error_text(&cfg));
            printf("Failed to open config file\n");
            return NULL;
        }
    }



    if(config_lookup_string(&cfg, key, &str)) {
        return str;
    } else {
        printf("No '%s' setting in configuration file.\n", key);
        return NULL;
    }
}



int get_int_value_for_key(char* key)
{
    config_init(&cfg);
    config_setting_t *setting;

    int val;

    if(!config_read_file(&cfg, QFREE_CONFIG_LOCATION)) {

        if(!config_read_file(&cfg, "../../config.cfg"))
        {

            printf("%s:%d - %s\n", config_error_file(&cfg),
                   config_error_line(&cfg), config_error_text(&cfg));
            printf("Failed to open config file\n");
            return 0;
        }
    }



    if(config_lookup_int(&cfg, key, &val)) {
        return val;
    } else {
        printf("No '%s' setting in configuration file.\n", key);
        return 0;
    }
}



config_setting_t *get_element_for_key(char* key)
{
    config_init(&cfg);
    config_setting_t *setting;

    if(!config_read_file(&cfg, QFREE_CONFIG_LOCATION)) {

        if(!config_read_file(&cfg, "../../config.cfg"))
        {

            printf("%s:%d - %s\n", config_error_file(&cfg),
                   config_error_line(&cfg), config_error_text(&cfg));
            printf("Failed to open config file\n");
            return 0;
        }
    }


    setting = config_lookup(&cfg, key);
    if(setting != NULL) {
        return setting;
    } else {
        printf("No '%s' setting in configuration file.\n", key);
        return 0;
    }
}


#ifdef __cplusplus
}
#endif
