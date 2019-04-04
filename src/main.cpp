#include <iostream>
#include <string>

#include <signal.h>
#include <unistd.h>
#include <cstring>

/* Mqtt library from contactless */
#include <wbmqtt/mqtt_wrapper.h>

#include "iec104_server.h"
#include "mqtt_client.h"

#include "config_parser.h"
#include "log.h"

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

    /* get the connection parameters - we need them to create correct ASDUs */
    CS101_AppLayerParameters alParams = iec104Server::Instance().getAppParams();

    iec104Server::Instance().start();

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