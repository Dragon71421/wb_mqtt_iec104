#include "config_parser.h"

#include <string>
#include <fstream>
#include <iostream>

#include <unistd.h>

#include <jsoncpp/json/json.h>

#include "mqtt_logger.h"

static Json::Value Root;

static void applyMqttSettings();

void parseCommandLineArgs(int argc, char *argv[])
{
    std::string config_file;
    int verbose_level = 0;
    int opt;
    
    while ((opt = getopt(argc, argv, "hc:v")) != -1) {
        switch (opt) {
        case 'c':
            config_file = optarg;
            break;
        case 'v':
            verbose_level++;
            break;
        case 'h':
        default:
            throw ConfigParserException(std::string("Usage: ") + argv[0] + " -c \"config_file_name\" [-v [-v ...]]");
        }
    }

    Json::Reader reader;

    std::ifstream fileStream(config_file);
    
    if (!fileStream.is_open())
        throw ConfigParserException("Can't open configuration file: " + config_file);

    if (!reader.parse(fileStream, Root, false))
        throw ConfigParserException( std::string("Can't parse input JSON file: ") +
                                     reader.getFormattedErrorMessages() );

    if( Root["ProtocolTrace"].isUInt() )
    {
        LogLevels log_level;
        if ( Root["ProtocolTrace"] >= LogLevels::MAX )
            log_level = LogLevels::ALL;
        else 
            log_level = (LogLevels) Root["ProtocolTrace"].asInt();
        
        MqttLogger::Instance().setLogLevel( log_level );
    }
    else
    {
        MqttLogger::Instance().setLogLevel( LogLevels::OFF );
    }
    

    if( Root["Mqtt"].isObject() )
    {
        applyMqttSettings();
    }
}

void applyMqttSettings()
{
    std::string ip = Root["Mqtt"]["Address"].asString();
    int port = Root["Mqtt"]["IPPort"].asInt();
    int keepAlive = Root["Mqtt"]["KeepAlive"].asInt();

    MqttLogger::Instance().setupMqttServer( ip, port, keepAlive );
}
