#pragma once

#include <string>

#include <wbmqtt/mqtt_wrapper.h>

#include "log.h"

/*!
 * Class that alow setup mqtt connection for print log to topic "IEC104Server/ProtocolTrace"
 * Use singlton pattern. 
 * 
 * Hide constructor, destructor and copying operators for singleton object. 
 * It guarantee that object can be created only once in Instance() function
 */
class MqttClient
{
public:
    static MqttClient& Instance()
    {
        static MqttClient s;
        return s;
    }

    void setupMqttServer( std::string & host, int port, int keepalive );

private:
    /* Hidden constructor and destructor */
    MqttClient();
    ~MqttClient() {}

    /* Hidden coppying operators */
    MqttClient(MqttClient const&);
    MqttClient& operator= (MqttClient const&);

    PMQTTClient mqtt_client;    /* Shared pointer to a mqtt_client */
};