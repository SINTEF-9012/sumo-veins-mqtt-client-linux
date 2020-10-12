//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

//Phu
//#include "veins/base/utils/Coord.h"

#ifdef __cplusplus
extern "C"
{
#endif

// C header here
//#include <mysql/mysql.h>
#include "zconf.h"
#include "pthread.h"
#include "MQTTClient.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "veins/modules/application/traci/config.h"
#include "veins/modules/application/traci/logger.h"
#include "veins/modules/application/traci/json_handler.h"
#include "veins/modules/application/traci/conversions.h"
#include "signal.h"
#include "stdbool.h"
//#include "db/db_access.h"
#include "veins/modules/application/traci/tile.h"

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
//void send_cams(Coord arg);
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


#ifdef __cplusplus
}
#endif


using namespace veins;

Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }

    //Phu: init for MQTT config
    init_config_pub();

    init_logger();
    print_console(LOG_TYPE_DEBUG, "....::::: C-MQTT-Pub Version %s :::::....\n",VERSION);
    start_mqtt();

}

void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    findHost()->getDisplayString().setTagArg("i", 1, "green");

    if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
    if (!sentMessage) {
        sentMessage = true;
        // repeat the received traffic update once in 2 seconds plus some random delay
        wsm->setSenderAddress(myId);
        wsm->setSerial(3);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }
}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
    if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial() + 1);
        if (wsm->getSerial() >= 3) {
            // stop service advertisements
            stopService();
            delete (wsm);
        }
        else {
            scheduleAt(simTime() + 1, wsm);
        }
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    //TODO Phu: call MQTT clients to send CAM messages
    Coord currentPosition = mobility->getPositionAt(simTime());

    /*

    struct vehicle_data_t *vehicles= (vehicle_data_t*) malloc(sizeof(*vehicles));
    vehicles = (vehicle_data_t*) realloc(vehicles, sizeof(*vehicles));

    int count = 0;

    vehicles[count].id = mobility->getId();

    char *message = (char*) currentPosition.info().data();//may cause error while converting?
    vehicles[count].message = (char*) malloc(strlen(message)+1);
    string_copy_local(message,vehicles[count].message);
    //strcpy(message,vehicles[count].message);

    vehicles[count].datetime = strdup("");
    vehicles[count].mobile_synced = 2;

    size_t current_length = 2;
    char *msg_json_array = (char*)malloc(current_length * sizeof(char));

    strcpy(msg_json_array,"[");

    if (vehicles[count].id > 0 && vehicles[count].message != NULL) {

        size_t next_size = strlen(vehicles[count].message) + current_length + 1;
        if (next_size > current_length) {
            char *temp = (char*)realloc(msg_json_array, (int)next_size * sizeof(char));
            if (temp == NULL) {
                print_console(LOG_TYPE_ERROR,"Out of memory\n");
            }
            msg_json_array = temp;
            current_length = next_size;
        }
        strcat(msg_json_array,vehicles[count].message);


        free(vehicles[count].message);

    }

    strcat(msg_json_array,"]");
    send_message(msg_json_array, 1);
    //set_server_batch_synced(ids, vehicle_b.count);

    free(vehicles);

    */

    // stopped for at least 10s?
    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);
            wsm->setDemoData(mobility->getRoadId().c_str());

            // host is standing still due to crash
            if (dataOnSch) {
                startService(Channel::sch2, 42, "Traffic Information Service");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            }
            else {
                // send right away on CCH, because channel switching is disabled
                sendDown(wsm);
            }
        }
    }
    else {
        lastDroveAt = simTime();
    }
}

//Phu
#ifdef __cplusplus
extern "C"
{
#endif

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

    //Phu not sending cams here but later
    //while (continue_publishing)
    //{
    //    send_cams();

        //send_non_cams();

    //    __useconds_t interval = (__useconds_t)get_publishig_interval();
    //    usleep(interval * 1000);
    //}
}

void create_mqtt_client()
{
    int client_created = -1;
    while (client_created == -1) {
        char *address = get_mqtt_server();
        char *client_id = get_mqtt_client_id_pub();
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
    char *tile = NULL; //get_tile_for_zoom_level4pub(maintile,zoom_level);
    free(maintile);
    char *topic = NULL; //get_tile_based_topic4pub(zoom_level, tile);
    free(tile);
    int qos = get_mqtt_pub_qos_local(isCam ? ITS_MESSAGEID_CAM : ITS_MESSAGEID_UNKNOWN);
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


#ifdef __cplusplus
}
#endif
