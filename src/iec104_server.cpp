#include "iec104_server.h"

#include <iostream>

#include "log.h"

static void printCP56Time2a(CP56Time2a time)
{
    MQTT_LOG_PRINTF( LogLevels::DEBUG, "%02i:%02i:%02i %02i/%02i/%04i", CP56Time2a_getHour(time),
                                                                        CP56Time2a_getMinute(time),
                                                                        CP56Time2a_getSecond(time),
                                                                        CP56Time2a_getDayOfMonth(time),
                                                                        CP56Time2a_getMonth(time),
                                                                        CP56Time2a_getYear(time) + 2000);
}

static bool clockSyncHandler (void* parameter, IMasterConnection connection, CS101_ASDU asdu, CP56Time2a newTime)
{
    MQTT_LOGGER( LogLevels::DEBUG, "Process time sync command with time "); printCP56Time2a(newTime);

    /* Set time for ACT_CON message */
    CP56Time2a_setFromMsTimestamp(newTime, Hal_getTimeInMs());

    /* update system time here */

    return true;
}

static bool interrogationHandler(void* parameter, IMasterConnection connection, CS101_ASDU asdu, uint8_t qoi)
{
    MQTT_LOG_PRINTF( LogLevels::DEBUG, "Received interrogation for group %i", qoi );

    if (qoi == 20) { /* only handle station interrogation */
        IMasterConnection_sendACT_CON(connection, asdu, false);

        IMasterConnection_sendACT_TERM(connection, asdu);
    }
    else {
        IMasterConnection_sendACT_CON(connection, asdu, true);
    }

    return true;
}

static bool asduHandler(void* parameter, IMasterConnection connection, CS101_ASDU asdu)
{
    if (CS101_ASDU_getTypeID(asdu) == C_SC_NA_1) {
        MQTT_LOGGER( LogLevels::DEBUG, "received single command\n");

        if  (CS101_ASDU_getCOT(asdu) == CS101_COT_ACTIVATION) {
            InformationObject io = CS101_ASDU_getElement(asdu, 0);

            if (InformationObject_getObjectAddress(io) == 5000) {
                SingleCommand sc = (SingleCommand) io;

                MQTT_LOG_PRINTF( LogLevels::DEBUG, "IOA: %i switch to %i\n", InformationObject_getObjectAddress(io),
                        SingleCommand_getState(sc));

                CS101_ASDU_setCOT(asdu, CS101_COT_ACTIVATION_CON);
            }
            else
                CS101_ASDU_setCOT(asdu, CS101_COT_UNKNOWN_IOA);

            InformationObject_destroy(io);
        }
        else
            CS101_ASDU_setCOT(asdu, CS101_COT_UNKNOWN_COT);

        IMasterConnection_sendASDU(connection, asdu);

        return true;
    }

    return false;
}

static bool connectionRequestHandler(void* parameter, const char* ipAddress)
{
    MQTT_LOG_PRINTF( LogLevels::DEBUG, "New connection request from %s\n", ipAddress);

    /* Example of accepting connection only from one ip address */
#if 0
    if (strcmp(ipAddress, "127.0.0.1") == 0) {
        printf("Accept connection\n");
        return true;
    }
    else {
        printf("Deny connection\n");
        return false;
    }
#else
    return true;
#endif
}

static void connectionEventHandler(void* parameter, IMasterConnection con, CS104_PeerConnectionEvent event)
{
    if (event == CS104_CON_EVENT_CONNECTION_OPENED) {
        MQTT_LOG_PRINTF( LogLevels::DEBUG, "Connection opened (%p)\n", (void*)con);
    }
    else if (event == CS104_CON_EVENT_CONNECTION_CLOSED) {
        MQTT_LOG_PRINTF( LogLevels::DEBUG, "Connection closed (%p)\n", (void*)con);
    }
    else if (event == CS104_CON_EVENT_ACTIVATED) {
        MQTT_LOG_PRINTF( LogLevels::DEBUG, "Connection activated (%p)\n", (void*)con);
    }
    else if (event == CS104_CON_EVENT_DEACTIVATED) {
        MQTT_LOG_PRINTF( LogLevels::DEBUG, "Connection deactivated (%p)\n", (void*)con);
    }
}


/*!
 * iec104Server constructor create iec104 server with default settings. 
 */
iec104Server::iec104Server()
{
    server = CS104_Slave_create(100, 100);

    /* Set mode to a single redundancy group
     * NOTE: library has to be compiled with CONFIG_CS104_SUPPORT_SERVER_MODE_SINGLE_REDUNDANCY_GROUP enabled (=1)
     */
    CS104_Slave_setServerMode(server, CS104_MODE_CONNECTION_IS_REDUNDANCY_GROUP);

    /* set the callback handler for the clock synchronization command */
    CS104_Slave_setClockSyncHandler(server, clockSyncHandler, NULL);

    /* set the callback handler for the interrogation command */
    CS104_Slave_setInterrogationHandler(server, interrogationHandler, NULL);

    /* set handler for other message types */
    CS104_Slave_setASDUHandler(server, asduHandler, NULL);

    /* set handler to handle connection requests (optional) */
    CS104_Slave_setConnectionRequestHandler(server, connectionRequestHandler, NULL);

    /* set handler to track connection events (optional) */
    CS104_Slave_setConnectionEventHandler(server, connectionEventHandler, NULL);
}

iec104Server::~iec104Server()
{
    CS104_Slave_stop(server);
    CS104_Slave_destroy(server);
}


void iec104Server::sendAsdu( CS101_ASDU& asdu )
{
    /* Add asdu for sending if server is running and drop asdu in other case */
    if( this->isRunning() )
        CS104_Slave_enqueueASDU(server, asdu);
}