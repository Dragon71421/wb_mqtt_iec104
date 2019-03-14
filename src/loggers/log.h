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

#define MQTT_LOGGER( lvl, msg )  MqttLogger::Instance().printLog( (lvl), (msg) );