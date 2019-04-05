#pragma once

#include <stdint.h>

void  addTopicForMonitor( const char* topic, uint16_t ioa );
bool isTopicMonitored( const char* topic );
uint16_t getTopicIoa( const char* topic );

void sendMonitorTI( uint16_t ioa, float value );