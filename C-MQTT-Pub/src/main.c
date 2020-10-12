//
// Created by root on 5/2/18.
//

#ifdef __cplusplus
extern "C"
{
#endif

// C header here
#include <mysql/mysql.h>
#include <zconf.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "MQTTClient.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <C-MQTT-Pub/src/config.h>
#include "../../log/logger.h"
#include <C-MQTT-Pub/src/json/json_handler.h>
#include "../helpers/conversions.h"
#include <signal.h>
#include <stdbool.h>
//#include "db/db_access.h"
#include <C-MQTT-Pub/src/tile.h>


int send_message(char *message,int isCam);
void connlost(void *context, char *cause);
void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void create_mqtt_client();
void start_mqtt();
void set_callbacks();
void mqtt_connect(MQTTClient_connectOptions* options);
int publish_message(char *veh_data, MQTTClient_deliveryToken *token,int isCam);
void mqtt_disconnect();
void* start_client(void *arg);
void send_cams();
//void send_non_cams();

char *get_tile();

volatile MQTTClient_deliveryToken deliveredtoken;
int continue_publishing = true;
MQTTClient client;

struct vehicle_data_t {
    int id;
    char *message;
    char *datetime;
    int mobile_synced;
};

struct vehicle_bundle_t {
    struct vehicle_data_t *vehicles;
    int count;
};

#define DEFAULT_VEHICLE_BUNDLE  {NULL, 0};

void signal_received(int signal)
{
    print_console(LOG_TYPE_DEBUG,"signal %d received\n",signal);
    if(signal == SIGINT)
        continue_publishing = false;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_received);

    init_config();

    init_logger();

    print_console(LOG_TYPE_DEBUG, "....::::: C-MQTT-Pub Version %s :::::....\n",VERSION);


    pthread_t client_thread_id;
    pthread_create(&client_thread_id, NULL, &start_client, NULL);

    while (continue_publishing) {
        sleep(2);
    }

    pthread_join(client_thread_id, NULL);

    mqtt_disconnect();
    deinit_logger();
    deinit_config();

    return 0;
}

void* start_client(void *arg)
{
    start_mqtt();
    pthread_exit(NULL);
}

int send_message(char *message,int isCam)
{
    MQTTClient_deliveryToken token;

    deliveredtoken = 0;

    int published = publish_message(message,&token,isCam);

    free(message);

//    while(deliveredtoken != token);

    if(published == 0) {
        return 0;
    } else {
        return -1;
    }
}

int publish_message(char *veh_data, MQTTClient_deliveryToken *token,int isCam)
{
    char *maintile = get_tile();
    int zoom_level = get_pub_zoom_level();
    char *tile = get_tile_for_zoom_level(maintile,zoom_level);
    free(maintile);
    char *topic = get_tile_based_topic(zoom_level, tile);
    free(tile);
    int qos = get_mqtt_pub_qos(isCam ? ITS_MESSAGEID_CAM : ITS_MESSAGEID_UNKNOWN);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = veh_data;
    pubmsg.payloadlen = (int)strlen(veh_data);
    pubmsg.qos = qos;
    pubmsg.retained = 1;

    print_console(LOG_TYPE_DEBUG,"Topic\t: %s\n",topic);
    print_console(LOG_TYPE_DEBUG,"QoS\t\t: %d\n",qos);
    print_console(LOG_TYPE_DEBUG,"Message\t:%s\n",veh_data);

    int ret = MQTTClient_publishMessage(client, topic, &pubmsg, token);

    if (ret == MQTTCLIENT_SUCCESS) {
        print_console(LOG_TYPE_DEBUG,"Message published successfully\n");
        free(topic);
        return 0;
    } else {
        print_console(LOG_TYPE_ERROR,"Publishing message failed. Error code: %d\n Trying again\n",ret);
        free(topic);
        return 0;
    }
}

char *get_tile() {
    return NULL;
}

void start_mqtt()
{

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    if(get_ssl_enabled() == 1) {
        MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
        ssl_opts.trustStore = get_ssl_capath();
        ssl_opts.enableServerCertAuth = 1;
        conn_opts.ssl = &ssl_opts;
    }

    char *username = get_mqtt_username();
    char *password = get_mqtt_password();

    if(strcmp(username,"none") != 0) {
        print_console(LOG_TYPE_DEBUG,"Activating authentication\n");
        conn_opts.username = username;
        conn_opts.password = password;
    } else{
        print_console(LOG_TYPE_DEBUG,"Connecting without authentication\n");
    }

    create_mqtt_client();

    set_callbacks();

    mqtt_connect(&conn_opts);

    while (continue_publishing)
    {
        send_cams();

        //send_non_cams();

        __useconds_t interval = (__useconds_t)get_publishig_interval();
        usleep(interval * 1000);
    }
}

struct vehicle_bundle_t get_cam_message()
{

    //print_console(LOG_TYPE_QUERY, "[get_last_server_synced_message] %s\n",query);

    int rows = 1;
    struct vehicle_data_t *vehicles=malloc(sizeof(*vehicles));
    vehicles = realloc(vehicles,rows*sizeof(*vehicles));

    int count =0;

        vehicles[count].id = 1;

        char *message = "test";
        vehicles[count].message = malloc(strlen(message)+1);
        string_copy(message,vehicles[count].message);

        vehicles[count].datetime = "";
        vehicles[count].mobile_synced = 2;


    struct vehicle_bundle_t b = {NULL,0};
    b.vehicles = vehicles;
    b.count = count;

    return b;
}

struct vehicle_bundle_t get_noncam_message()
{
    int rows = 1;

    struct vehicle_data_t *vehicles=malloc(sizeof(*vehicles));
    vehicles = realloc(vehicles,rows*sizeof(*vehicles));

    struct vehicle_bundle_t b = {NULL,0};
    b.vehicles = vehicles;
    b.count = 1;

    return b;
}

void send_cams()
{
    struct vehicle_bundle_t vehicle_b = get_cam_message();
    if(vehicle_b.count <= 0) {
        __useconds_t interval = (__useconds_t)get_publishig_interval();
        usleep(interval * 1000);
        return;
    }
    struct vehicle_data_t *vehicles = vehicle_b.vehicles;

    size_t current_length = 2;
    char *msg_json_array = (char*)malloc(current_length * sizeof(char));

    strcpy(msg_json_array,"[");

    int ids[vehicle_b.count];

    for (int i=0; i<vehicle_b.count; i++) {

        if (vehicles[i].id > 0 && vehicles[i].message != NULL) {
            ids[i] = vehicles[i].id;

            size_t next_size = strlen(vehicles[i].message) + current_length + 1;
            if (next_size > current_length) {
                char *temp = (char*)realloc(msg_json_array, (int)next_size * sizeof(char));
                if (temp == NULL) {
                    print_console(LOG_TYPE_ERROR,"Out of memory\n");
                }
                msg_json_array = temp;
                current_length = next_size;
            }
            strcat(msg_json_array,vehicles[i].message);
            if(i!=vehicle_b.count-1) {
                strcat(msg_json_array,",");
            }
            free(vehicles[i].message);
        }
    }

    strcat(msg_json_array,"]");
    send_message(msg_json_array, 1);
    //set_server_batch_synced(ids, vehicle_b.count);

    free(vehicles);
}

void send_non_cams()
{
    struct vehicle_bundle_t vehicle_b = get_noncam_message();
    if(vehicle_b.count <= 0) {
        __useconds_t interval = (__useconds_t)get_publishig_interval();
        usleep(interval * 1000);
        return;
    }
    struct vehicle_data_t *vehicles = vehicle_b.vehicles;

    size_t current_length = 2;
    char *msg_json_array = (char*)malloc(current_length * sizeof(char));

    strcpy(msg_json_array,"[");

    int ids[vehicle_b.count];

    for (int i=0; i<vehicle_b.count; i++) {

        if (vehicles[i].id > 0 && vehicles[i].message != NULL) {
            ids[i] = vehicles[i].id;

            size_t next_size = strlen(vehicles[i].message) + current_length + 1;
            if (next_size > current_length) {
                char *temp = (char*)realloc(msg_json_array, (int)next_size * sizeof(char));
                if (temp == NULL) {
                    print_console(LOG_TYPE_ERROR,"Out of memory\n");
                }
                msg_json_array = temp;
                current_length = next_size;
            }
            strcat(msg_json_array,vehicles[i].message);
            if(i!=vehicle_b.count-1) {
                strcat(msg_json_array,",");
            }
            free(vehicles[i].message);
        }
    }

    strcat(msg_json_array,"]");
    send_message(msg_json_array, 0);
    //set_server_batch_synced(ids, vehicle_b.count);

    free(vehicles);
}

void create_mqtt_client()
{
    int client_created = -1;
    while (client_created == -1) {
        char *address = get_mqtt_server();
        char *client_id = get_mqtt_client_id();
        int ret = MQTTClient_create(&client, address, client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);

        if (ret == MQTTCLIENT_SUCCESS) {
            client_created = 0;
            print_console(LOG_TYPE_DEBUG,"MQTT client created successfully\n");
            print_console(LOG_TYPE_DEBUG,"MQTT Address\t: %s\n", address);
            print_console(LOG_TYPE_DEBUG,"MQTT ClientID\t: %s\n", client_id);

            continue;
        } else {
            client_created = -1;
            print_console(LOG_TYPE_ERROR,"Creating MQTT client failed. Error code: %d\nTrying again\n",ret);

        }
        sleep(2);
    }
}

void set_callbacks()
{
    int callbacks_set = -1;
    while (callbacks_set == -1) {

        int ret = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

        if (ret == MQTTCLIENT_SUCCESS) {
            callbacks_set = 0;
            print_console(LOG_TYPE_DEBUG,"Callbacks successfully set.\n");
            continue;
        } else {
            callbacks_set = -1;
            print_console(LOG_TYPE_ERROR,"Setting callbacks failed. Error code: %d\n Trying again\n",ret);
        }
        sleep(2);
    }
}

void mqtt_connect(MQTTClient_connectOptions* options)
{
    int mqtt_connected = -1;
    while (mqtt_connected == -1) {
        int rc;
        if ((rc = MQTTClient_connect(client, options)) != MQTTCLIENT_SUCCESS) {
            print_console(LOG_TYPE_ERROR,"Failed to connect, Error code %d\n", rc);
            mqtt_connected = -1;

        } else {
            print_console(LOG_TYPE_DEBUG,"MQTT connected successfully\n");
            mqtt_connected = 0;
            continue;
        }
        sleep(2);
    }
}

void connlost(void *context, char *cause)
{
    pthread_t   tid;
    tid = pthread_self();
    print_console(LOG_TYPE_DEBUG,"---------------\nThreadid - connlost thread - %d\n--------------------\n",tid);

    print_console(LOG_TYPE_ERROR,"\nConnection lost\n");
    print_console(LOG_TYPE_ERROR,"cause: %s\n", cause);

    mqtt_disconnect();

    print_console(LOG_TYPE_DEBUG,"Starting Connection again\n");
    start_mqtt();
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    print_console(LOG_TYPE_DEBUG,"Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void mqtt_disconnect()
{
    pthread_t   tid;
    tid = pthread_self();
    print_console(LOG_TYPE_DEBUG,"---------------\nThreadid - disconnecting thread - %d\n--------------------\n",tid);

    print_console(LOG_TYPE_DEBUG, "Disconnecting MQTT\n");
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}


#ifdef __cplusplus
}
#endif