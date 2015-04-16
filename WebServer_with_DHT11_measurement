#include <dht11.h>
#include <EtherCard.h>

dht11 DHT11;

#define DHT11PIN 2
#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192,168,137,100 };
// gateway ip address
static byte gwip[] = { 192,168,137,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
BufferFiller bfill;

signed int temperatura;
unsigned int wilgotnosc;

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RBBB server</title>"
    "<h1>Jestem uruchomiony od $D$D:$D$D:$D$D</h1>"
    "<h1>Temperatura: $D st.C</h1>"
    "<h1>Wilgotonosc: $D %</h1>"),
      h/10, h%10, m/10, m%10, s/10, s%10,temperatura,wilgotnosc);
  return bfill.position();
}

void setup(){
  Serial.begin(57600);
  Serial.println("\n[backSoon]");
  
  Serial.println("DHT11 Sensor ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
}

void loop(){
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data

static int licznik = 0;
licznik ++;
if(!(licznik%10000))
{
  int chk = DHT11.read(DHT11PIN);
  Serial.println("\n");
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
		//Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		//Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		//Serial.println("Time out error"); 
		break;
    default: 
		//Serial.println("Unknown error"); 
		break;
  }
  if(chk==DHTLIB_OK)
  {
    wilgotnosc = (int)DHT11.humidity;
    temperatura = (int)DHT11.temperature;
    
    Serial.print("Humidity (%): ");
    Serial.println((float)DHT11.humidity, 2);
  
    Serial.print("Temperature (°C): ");
    Serial.println((float)DHT11.temperature, 2);
  
    Serial.print("Dew Point (°C): ");
    Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));
  
    Serial.print("Dew PointFast (°C): ");
    Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
  }
}
}


// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
	// (1) Saturation Vapor Pressure = ESGG(T)
	double RATIO = 373.15 / (273.15 + celsius);
	double RHS = -7.90298 * (RATIO - 1);
	RHS += 5.02808 * log10(RATIO);
	RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
	RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
	RHS += log10(1013.246);

        // factor -3 is to adjust units - Vapor Pressure SVP * humidity
	double VP = pow(10, RHS - 3) * humidity;

        // (2) DEWPOINT = F(Vapor Pressure)
	double T = log(VP/0.61078);   // temp var
	return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
	double a = 17.271;
	double b = 237.7;
	double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
	double Td = (b * temp) / (a - temp);
	return Td;
}

