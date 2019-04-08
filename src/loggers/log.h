#pragma once

enum LogLevels
{
    OFF = 0,
    ALL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE,
    MAX
};

#include "mqtt_logger.h"

#define MQTT_LOG_PRINTF( lvl, msg, ... ) {                                                                  \
                                            char msg_buf[256];                                              \
                                            if( snprintf( msg_buf, sizeof(msg_buf), (msg), __VA_ARGS__ ) )  \
                                                MQTT_LOGGER( (lvl), msg_buf);                               \
                                         };

#define MQTT_LOGGER( lvl, msg )  MqttLogger::Instance().printLog( (lvl), (msg) );