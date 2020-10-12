//
// Created by root on 4/27/18.
//

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef C_MQTT_Pub_CONFIG_H
#define C_MQTT_Pub_CONFIG_H



struct topic_t {
    char **topics;
    int size;
};

#define DEFAULT_TOPIC_INITIALIZER               {NULL, 0};


#define VERSION                                 "1.5"

/* logging */
#define LOG_DIR_DEV                             "/var/log/e8_borealis"
#define APP_DIR_DEV                             "/var/log/e8_borealis/MQTT_Pub"
#define LOG_DIR_ALL_DEV                         "/var/log/e8_borealis/MQTT_Pub/all"
#define LOG_DIR_MESSAGES_READ_DEV               "/var/log/e8_borealis/MQTT_Pub/messagesread"
#define LOG_DIR_MESSAGES_WRITE_DEV              "/var/log/e8_borealis/MQTT_Pub/messageswrite"
#define LOG_DIR_ERROR_DEV                       "/var/log/e8_borealis/MQTT_Pub/errors"
#define LOG_DIR_QUERY_DEV                       "/var/log/e8_borealis/MQTT_Pub/queries"
#define LOG_DIR_DEBUG_DEV                       "/var/log/e8_borealis/MQTT_Pub/debug"

#define LOG_DIR_QFREE                           "/userdata/log/e8_borealis"
#define APP_DIR_QFREE                           "/userdata/log/e8_borealis/MQTT_Pub"
#define LOG_DIR_ALL_QFREE                       "/userdata/log/e8_borealis/MQTT_Pub/all"
#define LOG_DIR_MESSAGES_READ_QFREE             "/userdata/log/e8_borealis/MQTT_Pub/messagesread"
#define LOG_DIR_MESSAGES_WRITE_QFREE            "/userdata/log/e8_borealis/MQTT_Pub/messageswrite"
#define LOG_DIR_ERROR_QFREE                     "/userdata/log/e8_borealis/MQTT_Pub/errors"
#define LOG_DIR_QUERY_QFREE                     "/userdata/log/e8_borealis/MQTT_Pub/queries"
#define LOG_DIR_DEBUG_QFREE                     "/userdata/log/e8_borealis/MQTT_Pub/debug"

#define QFREE_CONFIG_LOCATION                   "/userdata/var/config/MQTT/config.cfg"

#define LOG_CATEGORY_ALL                        "aventi.e8borealis.mqttpub"
#define LOG_CATEGORY_MESSAGES_READ              "aventi.e8borealis.mqttpub.messages.read"
#define LOG_CATEGORY_MESSAGES_WRITE             "aventi.e8borealis.mqttpub.messages.write"
#define LOG_CATEGORY_ERROR                      "aventi.e8borealis.mqttpub.error"
#define LOG_CATEGORY_QUERIES                    "aventi.e8borealis.mqttpub.query"
#define LOG_CATEGORY_DEBUG                      "aventi.e8borealis.mqttpub.debug"

#define MQTT_CLIENT_ID                          "mqtt.publisher.client_id"
#define MQTT_TOPIC                              "mqtt.publisher.topic"
#define MQTT_SSL_ENABLED                        "mqtt.ssl.enable_ssl"
#define MQTT_SSL_CAPATH                         "mqtt.ssl.capath"
#define MQTT_QOS                                "mqtt.publisher.qos"
#define MQTT_QOS_CAM                            "mqtt.publisher.qos_cam"
#define PUB_INTERVAL                            "mqtt.publisher.pub_interval"
#define TOPIC_SUBSCRIBER_ENABLED                "mqtt.subscriber.topic_subscriber.enabled"
#define TOPIC_SUBSCRIBER_TOPICS                 "mqtt.subscriber.topic_subscriber.topics"

#define SINGLE_TILE_SUBSCRIBER_ENABLED          "mqtt.subscriber.single_tile_subscriber.enabled"
#define SINGLE_TILE_MAIN_ZOOMLEVEL              "mqtt.subscriber.single_tile_subscriber.main_zoom_level"
#define SINGLE_TILE_SUB_ZOOMLEVEL               "mqtt.subscriber.single_tile_subscriber.sub_zoom_level"

#define NINE_TILE_SUBSCRIBER_ENABLED            "mqtt.subscriber.nine_tile_subscriber.enabled"
#define NINE_TILE_MAIN_ZOOMLEVEL                "mqtt.subscriber.nine_tile_subscriber.main_zoom_level"
#define NINE_TILE_SUB_ZOOMLEVEL                 "mqtt.subscriber.nine_tile_subscriber.sub_zoom_level"

#define PUBLISHER_ZOOMLEVEL                     "mqtt.publisher.zoom_level"

#define ENVIRONMENT_DEV                         "dev"
#define ENVIRONMENT_QFREE                       "qfree"

#define ITS_MESSAGEID_UNKNOWN                   0
#define ITS_MESSAGEID_DENM                      1
#define ITS_MESSAGEID_CAM                       2
#define ITS_MESSAGEID_IVI                       6
#define ITS_MESSAGEID_SAM                       12
#define ITS_MESSAGEID_VEHICLE_TYPE              -3
#define ITS_MESSAGEID_VITS_LOCATION             -4
#define ITS_MESSAGEID_SIMPLE_CAM                -2
#define ITS_MESSAGEID_SIMPLE_DENM               -1
#define ITS_MESSAGEID_SIMPLE_IVI                -6
#define ITS_MESSAGEID_SIMPLE_SAM                -12


#define MESSAGETYPE_CAM                         "cam"
#define MESSAGETYPE_DENM                        "denm"
#define MESSAGETYPE_SAM                         "sam"
#define MESSAGETYPE_IVI                         "ivi"
#define MESSAGETYPE_UNKNOWN                     "ukn"

char* get_mqtt_server();
char* get_mqtt_username();
char* get_mqtt_password();
char* get_mqtt_client_id_pub();
int get_mqtt_pub_qos_local(int message_id);
int get_publishig_interval();
char* get_db_server();
char* get_db_user();
char* get_db_password();
char* get_db_name();
char* get_env();
int get_log_level();
int get_print_level();
void init_config_pub();
void deinit_config();
char* get_ssl_capath();
int get_ssl_enabled();
int get_pub_zoom_level();


#endif //C_MQTT_Pub_CONFIG_H


#ifdef __cplusplus
}
#endif
