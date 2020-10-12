//
// Created by root on 4/27/18.
//


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include <config.h>
#include <logger.h>
#include "json/json_handler.h"
#include "tiles.h"
#include <mysql/mysql.h>
#include <MQTTClient.h>
#include <zconf.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <helpers/string_utils.h>


void connlost(void *context, char *cause);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void delivered(void *context, MQTTClient_deliveryToken dt);
void start_mqtt();
void create_mqtt_client();
void mqtt_connect(MQTTClient_connectOptions* options);
void mqtt_disconnect();
void set_callbacks();
void st_run_listener();
void nt_run_listener();
void* st_start_listener(void *arg);
void* nt_start_listener(void *arg);
bool subscribe_to_topic(char *topic, int qos);
bool unsubscribe_from_topic(char *topic);

char *get_tile();

struct tiles_t subscribed_tiles = DEFAULT_SUBSCRIBED_TILES;
volatile MQTTClient_deliveryToken deliveredtoken;
MQTTClient client;
char *subscribedTopic = NULL;
bool continue_listening = true;
bool subscribe = true;

bool isConnected = false;
bool subscribing = false;
bool unsubscribing = false;


pthread_t single_tile_listener = 0;
pthread_t nine_tile_listener = 0;

void signal_received(int signal)
{
    print_console(LOG_TYPE_DEBUG,"signal %d received\n",signal);
    if(signal == SIGINT)
        subscribe = false;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_received);

    init_config();
    init_logger();

    print_console(LOG_TYPE_DEBUG, "....::::: C-MQTT-Sub Version %s :::::....\n",VERSION);

    start_mqtt();


    while(subscribe){
        sleep(2);
    }


    continue_listening = false;

    if(get_single_tile_subscriber_enabled() == 1) {
        pthread_join(single_tile_listener, NULL);
    }

    if(get_nine_tile_subscriber_enabled() == 1) {
        pthread_join(nine_tile_listener, NULL);
    }

    mqtt_disconnect();
    //destroy_db();
    deinit_logger();
    deinit_config();
    return 0;
}

void start_mqtt()
{

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.cleansession = true;
    if(get_ssl_enabled() == 1) {
        MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
        ssl_opts.trustStore = get_ssl_capath();
        ssl_opts.enableServerCertAuth = 1;
        conn_opts.ssl = &ssl_opts;
    }
    
    char *username = get_mqtt_username();
    char *password = get_mqtt_password();

    create_mqtt_client();

    if(strcmp(username,"none") != 0) {
        print_console(LOG_TYPE_DEBUG,"Activating authentication\n");
        conn_opts.username = username;
        conn_opts.password = password;
    } else{
        print_console(LOG_TYPE_DEBUG,"Connecting without authentication\n");
    }

    set_callbacks();

    mqtt_connect(&conn_opts);

    if(get_single_tile_subscriber_enabled() == 1) {
        print_console(LOG_TYPE_DEBUG,"Single tile subscriber is enabled.\n");
        pthread_create(&single_tile_listener,NULL,st_start_listener,NULL);
    }

    if(get_nine_tile_subscriber_enabled() == 1) {
        print_console(LOG_TYPE_DEBUG,"Nine tile subscriber is enabled.\n");
        pthread_create(&nine_tile_listener,NULL,nt_start_listener,NULL);
    }

    if(get_topic_subscriber_enabled() == 1) {
        print_console(LOG_TYPE_DEBUG,"Topic subscriber is enabled.\n");
        struct topic_t topics = get_topics();
        print_console(LOG_TYPE_DEBUG, "No of topics = %d\n", topics.size);

        for(int i=0; i<topics.size; i++) {
            print_console(LOG_TYPE_DEBUG, "%s\n",topics.topics[i]);
            int qos = get_mqtt_sub_qos();
            subscribe_to_topic(topics.topics[i],qos);
        }
    }
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
            isConnected = false;
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
            print_console(LOG_TYPE_DEBUG,"Callbacks set successfully\n");
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
            isConnected = false;

        } else {
            print_console(LOG_TYPE_DEBUG,"\nMQTT connected successfully\n");
            mqtt_connected = 0;
            isConnected = true;
            continue;
        }
        sleep(2);
    }
}

bool subscribe_to_topic(char *topic, int qos)
{
    subscribing = true;
    int ret = MQTTClient_subscribe(client, topic, qos);

    if (ret == MQTTCLIENT_SUCCESS) {
        print_console(LOG_TYPE_DEBUG, "ADD - Subscribed to topic: %s using QoS%d\n",topic,qos);
        subscribing = false;
        return true;
    } else {
        print_console(LOG_TYPE_ERROR, "Subscribing to topic %s using QoS%d failed.\n",topic,qos);
        subscribing = false;
        return false;
    }
}

bool unsubscribe_from_topic(char *topic)
{
    unsubscribing = true;
    int ret = MQTTClient_unsubscribe(client,topic);

    if (ret == MQTTCLIENT_SUCCESS) {
        print_console(LOG_TYPE_DEBUG, "REMOVE - Unsubscribed from topic: %s\n",topic);
        unsubscribing = false;
        return true;
    } else {
        print_console(LOG_TYPE_ERROR, "Unsubscribing from topic %s failed.\n",topic);
        unsubscribing = false;
        return false;
    }
}


/*START - single tile subscriber*/
void* st_start_listener(void *arg)
{
    (void)arg;
    st_run_listener();
    return NULL;
}

void st_run_listener()
{
    subscribedTopic = malloc(2);
    strcpy(subscribedTopic,"1");
    subscribedTopic[1] = 0;

    while(continue_listening) {
        if(!isConnected) {
            sleep(2);
            continue;
        }

        if(subscribing || unsubscribing) {
            sleep(1);
            continue;
        }
        char *main_tile = get_tile();
        char *topic = get_tile_based_topic(get_single_tile_subZoom_level(),get_single_tile_mainZoom_level(), main_tile);
        free(main_tile);
        int qos = get_mqtt_sub_qos();
        bool isSuccess;
        if(topic != NULL)
        {
            if (strcmp(subscribedTopic,"1")==0)
            {
                isSuccess = subscribe_to_topic(topic,qos);
                if(isSuccess)
                {
                    subscribedTopic = realloc(subscribedTopic, strlen(topic)+1);
                    sprintf(subscribedTopic,"%s",topic);
                }
            }
            else if (strcmp(subscribedTopic, topic) == 0)
            {
            }
            else
            {
                isSuccess = unsubscribe_from_topic(subscribedTopic);
                if(isSuccess)
                {
                    isSuccess = subscribe_to_topic(topic,qos);
                    if(isSuccess)
                    {
                        subscribedTopic = realloc(subscribedTopic, strlen(topic)+1);
                        sprintf(subscribedTopic,"%s",topic);
                    }
                }
            }
        }
        else
        {
            print_console(LOG_TYPE_ERROR,"Topic not found in database\n");
        }
        free(topic);
        sleep(1);
    }

    print_console(LOG_TYPE_DEBUG,"Stopping single tile listener\n");
}

char *get_tile() {
    return NULL;
}
/*END - single tile subscriber*/




/*START - nine tile subscriber*/
bool nt_check_subscribed(char *quad)
{
    if(strcmp(quad,subscribed_tiles.center)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.top)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.top_right)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.right)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.bottom_right)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.bottom)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.bottom_left)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.left)==0) {
        return true;
    }
    else if(strcmp(quad,subscribed_tiles.top_left)==0) {
        return true;
    }
    return false;
}

bool nt_should_unsubscribe(char *quad, struct tiles_t current_tiles)
{
    if(strcmp(quad, current_tiles.center)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.top)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.top_right)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.right)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.bottom_right)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.bottom)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.bottom_left)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.left)==0) {
        return false;
    }
    else if(strcmp(quad, current_tiles.top_left)==0) {
        return false;
    }
    return true;
}

void nt_subscribe(char *quad, int qos, bool check)
{
    if(check && nt_check_subscribed(quad)) {
        return;
    }
    char *topic = get_tile_based_topic(get_nine_tile_subZoom_level(),get_nine_tile_mainZoom_level(), quad);
    subscribe_to_topic(topic,qos);
    free(topic);
}

void nt_check_and_unsubscribe(char *quad, struct tiles_t current_tiles)
{
    if(nt_should_unsubscribe(quad, current_tiles)){
        char *topic = get_tile_based_topic(get_nine_tile_subZoom_level(),get_nine_tile_mainZoom_level(), quad);
        unsubscribe_from_topic(topic);
        free(topic);
    }
}

void nt_run_listener()
{
    while(continue_listening) {
        if (!isConnected) {
            sleep(2);
            continue;
        }

        if (subscribing || unsubscribing) {
            sleep(1);
            continue;
        }

        if(subscribed_tiles.center == NULL){
            //first time
            char *main_tile =get_tile();
            char *current_center = get_tile_for_zoom_level(main_tile, get_nine_tile_subZoom_level());
            print_console(LOG_TYPE_DEBUG,"Current center %s\n",current_center);
            free(main_tile);
            struct tiles_t current_tiles = create_nine_tiles(current_center);
            int qos = get_mqtt_sub_qos();
            nt_subscribe(current_tiles.center,qos, false);
            nt_subscribe(current_tiles.top,qos, false);
            nt_subscribe(current_tiles.top_right,qos, false);
            nt_subscribe(current_tiles.right,qos, false);
            nt_subscribe(current_tiles.bottom_right,qos, false);
            nt_subscribe(current_tiles.bottom,qos, false);
            nt_subscribe(current_tiles.bottom_left,qos, false);
            nt_subscribe(current_tiles.left,qos, false);
            nt_subscribe(current_tiles.top_left,qos, false);

            subscribed_tiles = current_tiles;
            print_console(LOG_TYPE_DEBUG,"-------------\n");
        }
        else {
            char *main_tile =get_tile();
            char *current_center = get_tile_for_zoom_level(main_tile, get_nine_tile_subZoom_level());
            free(main_tile);

            if(strcmp(current_center, subscribed_tiles.center) == 0) {
                //same tile
                free(current_center);
            }
            else {
                struct tiles_t current_tiles = create_nine_tiles(current_center);
                int qos = get_mqtt_sub_qos();
                nt_subscribe(current_tiles.center,qos, true);
                nt_subscribe(current_tiles.top,qos, true);
                nt_subscribe(current_tiles.top_right,qos, true);
                nt_subscribe(current_tiles.right,qos, true);
                nt_subscribe(current_tiles.bottom_right,qos, true);
                nt_subscribe(current_tiles.bottom,qos, true);
                nt_subscribe(current_tiles.bottom_left,qos, true);
                nt_subscribe(current_tiles.left,qos, true);
                nt_subscribe(current_tiles.top_left,qos, true);

                nt_check_and_unsubscribe(subscribed_tiles.center, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.top, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.top_right, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.right, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.bottom_right, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.bottom, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.bottom_left, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.left, current_tiles);
                nt_check_and_unsubscribe(subscribed_tiles.top_left, current_tiles);

                destroy_tiles(subscribed_tiles);
                subscribed_tiles = current_tiles;
                print_console(LOG_TYPE_DEBUG,"-------------\n");
            }
        }

        sleep(1);
    }

    print_console(LOG_TYPE_DEBUG,"Stopping nine tile listener\n");
}


void* nt_start_listener(void *arg)
{
    (void)arg;
    nt_run_listener();
    return NULL;
}
/*END - nine tile subscriber*/


void connlost(void *context, char *cause)
{
    print_console(LOG_TYPE_ERROR,"\nConnection lost\n");
    print_console(LOG_TYPE_ERROR,"cause: %s\n", cause);
    print_console(LOG_TYPE_DEBUG,"Starting Connection again\n");
    mqtt_disconnect();
    start_mqtt();
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    if(message->payloadlen == 0){
        return 1;
    }
    int i;
    char* payloadptr;

    print_console(LOG_TYPE_DEBUG,"Message arrived\n");
    print_console(LOG_TYPE_DEBUG,"topic: %s\n", topicName);
    print_console(LOG_TYPE_DEBUG,"Length: %d,\n",message->payloadlen);
    print_console(LOG_TYPE_MESSAGE_READ,"message: ");


    char *db_message = malloc((size_t)message->payloadlen+2);
//    payloadptr = message->payload;
//    for(i=0; i<message->payloadlen; i++)
//    {
//        putchar(*payloadptr);
//        db_message[i] = *payloadptr;
//        payloadptr++;
//    }
    snprintf(db_message,(size_t)message->payloadlen+1,"%s",(char *)message->payload);
//    string_copy(message->payload, db_message, message->payloadlen);
    db_message[message->payloadlen+1] = '\0';
    putchar('\n');
    char *m = db_message;
    char *type = get_messagetype(db_message);
    char *stid = get_stationid(db_message);

    //struct vehicle_data_t vdata = vehicle_data_initializer;
    //vdata.message = m;
    //vdata.message_type = type;
    //vdata.station_id = stid;

    print_console(LOG_TYPE_DEBUG, "%s\n", db_message);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    //insert_server_record(vdata);
    return 1;
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    print_console(LOG_TYPE_DEBUG, "Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void mqtt_disconnect()
{
    print_console(LOG_TYPE_DEBUG, "Disconnecting MQTT\n");
    isConnected = false;
    free(subscribedTopic);
    destroy_tiles(subscribed_tiles);
    subscribed_tiles.center = NULL;
    subscribed_tiles.top = NULL;
    subscribed_tiles.top_right = NULL;
    subscribed_tiles.right = NULL;
    subscribed_tiles.bottom_right = NULL;
    subscribed_tiles.bottom = NULL;
    subscribed_tiles.bottom_left = NULL;
    subscribed_tiles.left = NULL;
    subscribed_tiles.top_left = NULL;
    continue_listening = false;
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}
