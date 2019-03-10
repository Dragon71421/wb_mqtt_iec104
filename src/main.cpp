#include <iostream>
#include <string>

#include <signal.h>
#include <unistd.h>
#include <cstring>

/* Mqtt library from contactless */
#include <wbmqtt/mqtt_wrapper.h>

#include "config_parser.h"
#include "log.h"


/* Global variables is wrong decision but temporary */
PMQTTClient mqtt_client;

/* Simple observer that print message with mqtt connection status */
class TestObserver : public IMQTTObserver {
public:    
    void OnConnect(int rc) override;
    void OnMessage(const struct mosquitto_message *message) override;
    void OnSubscribe(int mid, int qos_count, const int *granted_qos) override;
};

void TestObserver::OnConnect(int rc)
{
    MQTT_LOGGER( MqttLogger::L_DEBUG, "OnConnect" );
}

void TestObserver::OnMessage(const struct mosquitto_message *message)
{    
    /* Create string and append information about incoming message topic */
    std::string outMessage;
    outMessage.append( "Message from: " );
    outMessage.append( message->topic );

    /* Append incoming message */
    outMessage.append( "\nMessage: " );
    outMessage.append( static_cast<const char*>(message->payload), message->payloadlen );
    
    /* Publish string with info about message to TRACE_TOPIC */
    MQTT_LOGGER( MqttLogger::L_ERROR, outMessage.c_str() );
}

void TestObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos)
{
    MQTT_LOGGER( MqttLogger::L_INFO, "OnSubscribe" );
}

/* Flag for signal processing */
static bool running = true;

/* Simple signal handler. Just set flag false */
void sighndlr(int signal)
{
    running = false;
}

void set_sighandler()
{
    struct sigaction act;
    memset(&act, 0, sizeof (act));
    act.sa_handler = sighndlr;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    act.sa_mask = set;

    sigaction(SIGINT, &act, 0);
    sigaction(SIGTERM, &act, 0);
}


int main( int argc, char** argv )
{
    set_sighandler();

    try /* Parse command line arguments and apply them */
    {
        parseCommandLineArgs( argc, argv );
    }
    catch (const ConfigParserException& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    /* create test MQTT client just for example */
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Port = 1883;
    mqtt_config.Host = "192.168.1.38";
    mqtt_config.Id = "mqtt-iec104";
    mqtt_client = std::make_shared<TMQTTClient>(mqtt_config);

    /* add observer to client */
    mqtt_client->Observe( std::make_shared<TestObserver>() );

    mqtt_client->ConnectAsync();
    mqtt_client->StartLoop();

    mqtt_client->Subscribe( nullptr, "/devices/hwmon/controls/CPU Temperature" , 0);

    while(running)
    {
        /* Print test message every 5 seconds */
        sleep(5);
        MQTT_LOGGER( MqttLogger::L_DEBUG, "Test wb MQTT" );
    }

    return 0;
}