#include "mqtt_logger.h"

/*! constant name of topic with log messages */
const std::string TRACE_TOPIC = "/IEC104Server/ProtocolTrace";

/*!
 * MqttLogger constructor create mqtt logger with default settings. 
 * Connection to localhost:1883 with keepAlive 60
 */
MqttLogger::MqttLogger()
{
    log_level = LogLevels::OFF;

    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Id = "mqtt-iec104-logger";
    mqtt_client = std::make_shared<TMQTTClient>( mqtt_config );

    mqtt_client->ConnectAsync();
    mqtt_client->StartLoop();
}

void MqttLogger::setupMqttServer( std::string & host, int port, int keepAlive )
{
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Port = port;
    mqtt_config.Host = host;
    mqtt_config.Keepalive = keepAlive;
    mqtt_config.Id = "mqtt-iec104-logger";

    mqtt_client->StopLoop();
    mqtt_client = std::make_shared<TMQTTClient>( mqtt_config );

    mqtt_client->ConnectAsync();
    mqtt_client->StartLoop();
}

/*!
 * Print message if logging enabled and current log level bigger then message level 
 */
void MqttLogger::printLog( LogLevels msg_level, const char* msg )
{
    if( mqtt_client && (log_level != 0) ) /* Print log if mqtt client exist and log enable */
    {
        if( (log_level == LogLevels::ALL) || (log_level >= msg_level) )
        {
            mqtt_client->Publish( nullptr, TRACE_TOPIC, msg);
        }
    }
}