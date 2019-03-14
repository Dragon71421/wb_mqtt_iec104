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
class MqttLogger
{
public:
    static MqttLogger& Instance()
    {
        static MqttLogger s;
        return s;
    }

    void setupMqttServer( std::string & host, int port, int keepalive );
    void printLog( LogLevels msg_level, const char* msg );

    void setLogLevel( LogLevels level ) { log_level = level; }

private:
    /* Hidden constructor and destructor */
    MqttLogger();
    ~MqttLogger() {}

    /* Hidden coppying operators */
    MqttLogger(MqttLogger const&);
    MqttLogger& operator= (MqttLogger const&);

    
    PMQTTClient mqtt_client;    /* Shared pointer to a mqtt_client */
    LogLevels log_level;
};