#include <iostream>

#include <signal.h>
#include <cstring>

/* Mqtt library from contactless */
#include <wbmqtt/version.h>
#include <wbmqtt/mqtt_wrapper.h>

#include <jsoncpp/json/json.h>

/* Simple observer that print message with mqtt connection status */
class TestObserver : public IMQTTObserver {
public:    
    void OnConnect(int rc) override;
    void OnMessage(const struct mosquitto_message *message) override {}
    void OnSubscribe(int mid, int qos_count, const int *granted_qos) override {}
};

void TestObserver::OnConnect(int rc)
{
    std::cout << "Mqtt connect status: " << rc << std::endl;
}

/* Flag for signal processing */
bool running = true;

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

    std::cout << "Test MQTT" << std::endl;

    /* create MQTT client */
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Port = 1883;
    mqtt_config.Host = "localhost";
    mqtt_config.Id = "mqtt-iec104";
    PMQTTClient mqtt_client = std::make_shared<TMQTTClient>(mqtt_config);

    /* add observer to client */
    mqtt_client->Observe( std::make_shared<TestObserver>() );

    mqtt_client->ConnectAsync();
    mqtt_client->StartLoop();

    while(running)
    {

    };

    return 0;
};