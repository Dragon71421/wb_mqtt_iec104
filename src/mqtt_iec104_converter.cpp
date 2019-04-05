#include "mqtt_iec104_converter.h"

#include <map>

#include "iec104_server.h"
#include "mqtt_client.h"

static std::map <std::string, uint16_t> convertMap;

void  addTopicForMonitor( const char* topic, uint16_t ioa )
{
    convertMap.insert( {topic, ioa} );
    
    MqttClient::Instance().subscribe( topic );
}

bool isTopicMonitored( const char* topic )
{
    return convertMap.count( topic ) ? true : false;
}

uint16_t getTopicIoa( const char* topic )
{
    return convertMap.at( topic );
}

void sendMonitorTI( uint16_t ioa, float value )
{
    CS101_AppLayerParameters alParams = iec104Server::Instance().getAppParams();
    CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_SPONTANEOUS, 0, 1, false, false);

    InformationObject io = (InformationObject) MeasuredValueShort_create( NULL, ioa, value, IEC60870_QUALITY_GOOD);

    CS101_ASDU_addInformationObject(newAsdu, io);

    InformationObject_destroy(io);

    /* Add ASDU to slave event queue - don't release the ASDU afterwards!
        * The ASDU will be released by the Slave instance when the ASDU
        * has been sent.
        */
    iec104Server::Instance().sendAsdu( newAsdu );

    CS101_ASDU_destroy(newAsdu);
}