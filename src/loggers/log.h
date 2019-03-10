#pragma once

#include "mqtt_logger.h"

#define MQTT_LOGGER( lvl, msg )  MqttLogger::Instance().printLog( (lvl), (msg) );