#include "mqtt_client.h"

#include "test_observer.h"

/*!
 * MqttClient constructor create mqtt logger with default settings. 
 * Connection to localhost:1883 with keepAlive 60
 */
MqttClient::MqttClient()
{
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Id = "mqtt-iec104";
    mqtt_client = std::make_shared<TMQTTClient>( mqtt_config );
    mqtt_client->Observe( std::make_shared<TestObserver>() );

    mqtt_client->ConnectAsync();
    mqtt_client->StartLoop();
}

void MqttClient::setupMqttServer( std::string & host, int port, int keepAlive )
{
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Port = port;
    mqtt_config.Host = host;
    mqtt_config.Keepalive = keepAlive;
    mqtt_config.Id = "mqtt-iec104";

    mqtt_client->StopLoop();
    mqtt_client = std::make_shared<TMQTTClient>( mqtt_config );
    mqtt_client->Observe( std::make_shared<TestObserver>() );

    mqtt_client->ConnectAsync();
    mqtt_client->StartLoop();
}

void MqttClient::subscribe( const char* topic )
{
    mqtt_client->Subscribe( nullptr, topic , 0);
}