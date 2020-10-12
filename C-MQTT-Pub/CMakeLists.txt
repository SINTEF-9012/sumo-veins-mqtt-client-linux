cmake_minimum_required(VERSION 3.5.1)
project(C_MQTT_Pub C)

set(CMAKE_C_STANDARD 99)

include_directories(C_MQTT_Pub PRIVATE ./src ../log ../helpers)

add_executable(C_MQTT_Pub
        ../helpers/conversions.c
        ../log/logger.c
        src/main.c
        ../config/config.c
        src/json/json_handler.c
        src/json/json_handler.h
        src/helpers/string_utils.h
        src/helpers/string_utils.c
        src/tile.c
        src/tile.h)

target_link_libraries(C_MQTT_Pub PRIVATE mysqlclient paho-mqtt3cs config log4c pthread jansson)
