cmake_minimum_required(VERSION 3.5.1)
project(C_MQTT_Sub C)

set(CMAKE_C_STANDARD 99)

include_directories(C_MQTT_Pub PRIVATE ./src ../log ../helpers)

add_executable(C_MQTT_Sub
        src/main.c
        ../helpers/conversions.c
        ../log/logger.c
        ../config/config.c
        src/json/json_handler.c
        src/tiles.h
        src/tiles.c
        src/helpers/string_utils.c
        src/helpers/string_utils.h)

target_link_libraries(C_MQTT_Sub PRIVATE mysqlclient paho-mqtt3cs config log4c jansson pthread)
