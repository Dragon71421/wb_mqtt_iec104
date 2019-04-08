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
    
    iec104Server::Instance().start();

    MQTT_LOGGER( LogLevels::DEBUG, "Start main loop" );

    while(running)
    {
        
    }

    return 0;
}