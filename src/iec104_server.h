#pragma once

#include <lib60870/cs104_slave.h>
#include <lib60870/hal_time.h>
#include <lib60870/hal_thread.h>

struct sCS101_ASDU {
    CS101_AppLayerParameters parameters;
    uint8_t* asdu;
    int asduHeaderLength;
    uint8_t* payload;
    int payloadSize;
};


/*!
 * Class that alow run and config iec104 server side. This class provide interface 
 * for creating and adding information objects for iec104 protocol.
 * 
 * Hide constructor, destructor and copying operators for singleton object. 
 * It guarantee that object can be created only once in Instance() function
 */
class iec104Server
{
public:
    static iec104Server& Instance()
    {
        static iec104Server s;
        return s;
    }

    void setIpAddress(const char* ipAddress) { CS104_Slave_setLocalAddress(server, ipAddress); }
    CS101_AppLayerParameters getAppParams() { return CS104_Slave_getAppLayerParameters(server); }
    void sendAsdu( CS101_ASDU asdu);

    void start() { CS104_Slave_start(server); }
    void stop()  { CS104_Slave_stop(server); }
    bool isRunning() { return CS104_Slave_isRunning(server); }

private:
    /* Hidden constructor and destructor */
    iec104Server();
    ~iec104Server();

    /* Hidden coppying operators */
    iec104Server(iec104Server const&);
    iec104Server& operator= (iec104Server const&);

    CS104_Slave server;
};