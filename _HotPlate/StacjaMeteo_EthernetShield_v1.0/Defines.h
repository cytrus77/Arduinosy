#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG 1

//Publish
#define MQTT_WIND          3090
#define MQTT_RAIN          3091
#define MQTT_TEMP_DHT      3031
#define MQTT_TEMP_DS18     3030
#define MQTT_HUMIDEX       3092
#define MQTT_NOISE         3093
#define MQTT_LIGHT         3070
#define MQTT_PRESSURE      3094
#define MQTT_UPTIME        3000
//Subscribe
#define MQTT_DIMMER        3020


// PINOUT
#define WINDRXPIN            0
#define WINDTXPIN            1
#define DEBUGRXPIN           2  // Serial Rx for Debug
#define DEBUGTXPIN           3  // Serial Tx for Debug
//#define SSforSDCard        4 - reserved
#define DS18B20PIN           5
#define DIMMERPIN            6
#define DHT22PIN             7
#define RS485TXENPIN         8   //RS485 SerialTransmit Enable
#define RS485RXENPIN         9   //RS485 SerialReceive Enable
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define RAINPIN              A0
#define NOISEPIN             A1
#define LIGHTPIN             A2


// measurement period
#define TIMER0PERIOD         2000 // 2ms
#define COUNTIN1SEC          1000000/TIMER0PERIOD
#define WINDPERIOD           10   // 10s
#define RAINDPERIOD          10   // 10s
#define TEMPPERIOD           60   // 1min
#define NOISEPERIOD          10   // 10s
#define LIGHTPERIOD          10   // 10s
#define PRESSUREPERIOD       60   // 1min
#define UPTIMEPERIOD         30   // 30s


#define RS485TransmitOn     HIGH
#define RS485TransmitOff    LOW  
#define RS485ReceiveOn      LOW
#define RS485ReceiveOff     HIGH  

#endif


