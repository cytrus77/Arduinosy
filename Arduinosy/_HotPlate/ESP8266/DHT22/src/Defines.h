#define INT_TIMER_PERIOD   10000 // in us
#define TIMER_PERIOD_NODEMCU INT_TIMER_PERIOD * 80
#define CYCLES_PER_SECOND 1000000 / INT_TIMER_PERIOD

#define DIMMER_TIMEOUT  600  // in seconds

////////////////////////////////////////////////////////////////////////////////

//Pinout
// #define ADC             A0
// #define GPIO0           D0
#define SCL_PIN            D1
#define SDA_PIN            D2
#define DIMMER_PIN         D3
#define STATUS_PIN         D4
#define RELAY_PIN          D5
// #define GPIO6           D6
#define DS18B20_PIN        D7
#define OE_PIN             D8

//Publish
const String MQTT_UPTIME    = "/outdoor/uptime/dimmer";
const String MQTT_SUBSCRIBE = "/outdoor/subsribe/dimmer";

//Subscribe
const String MQTT_DIMMER         = "/outdoor/light/dimmer/value";
const String MQTT_DIMMET_TIMEOUT = "/outdoor/light/dimmer/timer";
const String MQTT_TEMP           = "/outdoor/temp/dimmer";
