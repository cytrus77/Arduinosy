#define INT_TIMER_PERIOD   10000 // in us
#define TIMER_PERIOD_NODEMCU INT_TIMER_PERIOD * 80
#define CYCLES_PER_SECOND 1000000 / INT_TIMER_PERIOD

#define DIMMER_TIMEOUT  600  // in seconds

////////////////////////////////////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        9003
#define MQTT_SUBSCRIBE     9050

//Subscribe
#define MQTT_DIMMER         9301
#define MQTT_DIMMET_TIMEOUT 9302

//Pinout
// #define NOTUSED         A0
// #define NOTUSED         D0
// #define NOTUSED         D1
// #define NOTUSED         D2
// #define NOTUSED         D3
// #define NOTUSED         D4 // BUILTIN LED
// #define NOTUSED         D5
// #define NOTUSED         D1
// #define NOTUSED         D7
// #define NOTUSED         D8
