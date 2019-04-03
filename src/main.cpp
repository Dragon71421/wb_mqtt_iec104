#include <iostream>
#include <string>

#include <signal.h>
#include <unistd.h>
#include <cstring>

/* Mqtt library from contactless */
#include <wbmqtt/mqtt_wrapper.h>
#include "iec104_server.h"

#include "config_parser.h"
#include "log.h"

#if 1
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
    MQTT_LOGGER( LogLevels::DEBUG, "OnConnect" );
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
    MQTT_LOGGER( LogLevels::ERROR, outMessage.c_str() );
}

void TestObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos)
{
    MQTT_LOGGER( LogLevels::INFO, "OnSubscribe" );
}
#endif

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

    iec104Server::Instance().setIpAddress("127.0.0.1");

    /* get the connection parameters - we need them to create correct ASDUs */
    CS101_AppLayerParameters alParams = iec104Server::Instance().getAppParams();//CS104_Slave_getAppLayerParameters(slave);

    iec104Server::Instance().start();
    
#if 1
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
#endif

    MQTT_LOGGER( LogLevels::DEBUG, "Start main loop" );
    int16_t scaledValue = 0;
    while(running)
    {
        /* Print test message every 5 seconds */
        sleep(5);
        MQTT_LOGGER( LogLevels::DEBUG, "Test wb MQTT" );

        CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_PERIODIC, 0, 1, false, false);
        InformationObject io = (InformationObject) MeasuredValueScaled_create(NULL, 110, scaledValue, IEC60870_QUALITY_GOOD);

        scaledValue++;

        CS101_ASDU_addInformationObject(newAsdu, io);

        InformationObject_destroy(io);

        /* Add ASDU to slave event queue - don't release the ASDU afterwards!
         * The ASDU will be released by the Slave instance when the ASDU
         * has been sent.
         */
        iec104Server::Instance().sendAsdu( newAsdu );

        CS101_ASDU_destroy(newAsdu);
    }

    return 0;
}