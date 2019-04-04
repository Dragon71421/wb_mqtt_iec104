#pragma once

#include <wbmqtt/mqtt_wrapper.h>

#include "log.h"
#include "mqtt_iec104_converter.h"

/* Simple observer that print message with mqtt connection status */
class TestObserver : public IMQTTObserver {
public:    
    void OnConnect(int rc) override;
    void OnMessage(const struct mosquitto_message *message) override;
    void OnSubscribe(int mid, int qos_count, const int *granted_qos) override;
};

void TestObserver::OnConnect(int rc)
{
    MQTT_LOGGER( LogLevels::DEBUG, "OnConnect" );
}

void TestObserver::OnMessage(const struct mosquitto_message *message)
{    
    /* Create string and append information about incoming message topic */
    std::string outMessage;
    outMessage.append( "Message from: " );
    outMessage.append( message->topic );

    std::string msg( static_cast<const char*>(message->payload), message->payloadlen );
    /* Append incoming message */
    outMessage.append( "\nMessage: " );
    outMessage.append( static_cast<const char*>(message->payload), message->payloadlen );
    
    /* Publish string with info about message to TRACE_TOPIC */
    MQTT_LOGGER( LogLevels::DEBUG, outMessage.c_str() );

    if( isTopicMonitored( message->topic ) )
    {
        /* Example of send value by iec104 */
        sendMonitorTI( 112, stof(msg) );
    }
}

void TestObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos)
{
    MQTT_LOGGER( LogLevels::INFO, "OnSubscribe" );
}
