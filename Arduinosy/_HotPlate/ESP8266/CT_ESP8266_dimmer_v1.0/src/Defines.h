#define INT_TIMER_PERIOD   10000 // in us
#define TIMER_PERIOD_NODEMCU INT_TIMER_PERIOD * 80
#define CYCLES_PER_SECOND 1000000 / INT_TIMER_PERIOD

#define DIMMER_TIMEOUT  0  // in seconds

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

// const String mqtt_login    = "outdoor";  // Default
// const String mqtt_login    = "outdoor_light_l";  // Mako
const String mqtt_login    = "outdoor_light_p";  // Mako
const String mqtt_user     = "admin";
const String mqtt_password = "Isb_C4OGD4c3";

// const String wifi_ssid     = "cytrynowa";
// const String wifi_password = "limonkowy";
// byte server[] = { 192, 168, 38, 11 };
const String wifi_ssid     = "M-net-d";
const String wifi_password = "123frytki";
byte server[] = { 192, 168, 11, 105 };

const String MQTT_DEVICE = mqtt_login;

//Publish
const String MQTT_UPTIME    = MQTT_DEVICE + "/uptime/dimmer";
const String MQTT_SUBSCRIBE = MQTT_DEVICE + "/subsribe/dimmer";
const String MQTT_CURRENT   = MQTT_DEVICE + "/current";
const String MQTT_VOLTAGE   = MQTT_DEVICE + "/voltage";
const String MQTT_POWER     = MQTT_DEVICE + "/power";

//Subscribe
const String MQTT_DIMMER         = MQTT_DEVICE + "/light/dimmer/value";
const String MQTT_DIMMET_TIMEOUT = MQTT_DEVICE + "/light/dimmer/timer";
const String MQTT_TEMP           = MQTT_DEVICE + "/temp/dimmer";
