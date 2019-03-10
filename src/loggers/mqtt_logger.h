#include <string>

#include <wbmqtt/mqtt_wrapper.h>

/*!
 * Class that alow setup mqtt connection for print log to topic "IEC104Server/ProtocolTrace"
 * Use singlton pattern. 
 * 
 * Hide constructor, destructor and copying operators for singleton object. 
 * It guarantee that object can be created only once in Instance() function
 */
class MqttLogger
{
public:
    static MqttLogger& Instance()
    {
        static MqttLogger s;
        return s;
    }

    void setupMqttServer( std::string & host, int port, int keepalive );
    void printLog( uint msg_level, const char* msg );

    enum LogLevels
    {
        L_OFF = 0,
        L_ALL,
        L_ERROR,
        L_WARN,
        L_INFO,
        L_DEBUG,
        L_TRACE,
        L_MAX
    };

    void setLogLevel( uint level ) { log_level = level; }

private:
    MqttLogger();
    ~MqttLogger() {}

    MqttLogger(MqttLogger const&);
    MqttLogger& operator= (MqttLogger const&);

    
    PMQTTClient mqtt_client;    /* Shared pointer to a mqtt_client */
    uint log_level;
};