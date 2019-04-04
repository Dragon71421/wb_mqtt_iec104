#include "config_parser.h"

#include <string>
#include <fstream>
#include <iostream>

#include <unistd.h>

#include <jsoncpp/json/json.h>

#include "mqtt_logger.h"

static Json::Value Root;

static void applyMqttSettings();

/*!
 * Function use getopt utils for parsing command line arguments.
 * If programm executed with '-c' optinons this function also try parse json file by jsoncpp
 * unction throw exception if there are unsupported or incorrect arguments from command line
 */
void parseCommandLineArgs(int argc, char *argv[])
{
    std::string config_file;
    int verbose_level = 0;
    int opt;
    
    /* Try recognize options from command line */
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

    /* Create json reader and try open and parse config_file. Throw exception on any error. */
    Json::Reader reader;
    std::ifstream fileStream(config_file);
    
    if (!fileStream.is_open())
        throw ConfigParserException("Can't open configuration file: " + config_file);

    if (!reader.parse(fileStream, Root, false))
        throw ConfigParserException( std::string("Can't parse input JSON file: ") +
                                     reader.getFormattedErrorMessages() );

    /* Enable logging if ProtocolTrace exist and is int. */
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
    
    /* Apply MQTT settings if they are defined in config_file. In other case default settings already setted up */
    if( Root["Mqtt"].isObject() )
    {
        applyMqttSettings();
    }
}

static void applyMqttSettings()
{
    std::string ip = Root["Mqtt"]["Address"].asString();
    int port = Root["Mqtt"]["IPPort"].asInt();
    int keepAlive = Root["Mqtt"]["KeepAlive"].asInt();

    MqttLogger::Instance().setupMqttServer( ip, port, keepAlive );
    MqttClient::Instance().setupMqttServer( ip, port, keepAlive );
}
