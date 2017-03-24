#include <SPI.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Defines.h"

void changeRelayState(SPort& port, ERelayState newState)
{
  if (port.relayState != newState)
  {
    digitalWrite(port.relayPin, newState == On ? false : true);
    port.relayState = newState;

    #ifdef DEBUG
      Serial.print("port: ");
      Serial.print(port.relayPin);
      Serial.print(" relayState:");
      Serial.println(port.relayState);
    #endif
  }
}

void changeShortCircuitState(SPort& port, EShortCircuitState newState)
{
  port.shortState = newState;

  switch (port.shortState)
  {
    case ShortCircuit2s:
    case ShortCircuit5s:
      port.timer = 1;
      changeRelayState(port, On);
      break;

    case ShortCircuitPerm:
      port.timer = 0;
      changeRelayState(port, On);
      break;

    case ShortCircuitOff:
    default:
      port.timer = 0;
      changeRelayState(port, Off);
      break;
  }

  #ifdef DEBUG
    Serial.print("port: ");
    Serial.print(port.relayPin);
    Serial.print(" shortState:");
    Serial.println(port.shortState);
  #endif

  addRefeshSection = true;
}

float getDcVoltage(SPort& port)
{
  int sensorReading = analogRead(port.dcVolPin);
  float dcVoltage = static_cast<float>(sensorReading)*2/100; ///1024*20;    // sensorValue/1024   - 1024=5V ->

  return dcVoltage;
}

void httpRefreshSection(EthernetClient& client)
{
  if (addRefeshSection)
  {
    client.print("<meta http-equiv=\"Refresh\" content=\"0; url=http://");
    client.print(ip[0]);
    client.print(".");
    client.print(ip[1]);
    client.print(".");
    client.print(ip[2]);
    client.print(".");
    client.print(ip[3]);
    client.println("\" />");

    addRefeshSection = false;
  }
}

void httpPortSection(EthernetClient& client, SPort& port, int portNo)
{
  client.print("<h2>Port: ");
  client.print(portNo);
  client.println("</h2>");
  client.print("<a href=\"/?");
  client.print(portNo);
  client.println("_Off\"\">1. Normal</a>");
  client.print("<a href=\"/?");
  client.print(portNo);
  client.println("_2s\"\">2. Short 2s</a>");
  client.print("<a href=\"/?");
  client.print(portNo);
  client.println("_5s\"\">3. Short 5s</a>");
  client.print("<a href=\"/?");
  client.print(portNo);
  client.println("_Perm\"\">4. Permanent short</a><br />");
  client.println("<br />");
  // DC Voltage section
  client.print("<a style=\"background-color:#4DB257\">DC Voltage = ");  //
  client.print(getDcVoltage(port));
  client.println("V </a>");
  // Short circuit section
  client.print("<a style=\"background-color:#4DB257\">Short circuit mode: ");  //
  client.print(port.shortState);
  client.println("</a>");
  client.println("<br />");
}

bool httpReadButton(String& readString, SPort& port, int portNo)
{
  bool ret = false;
  String portStr = String(portNo, DEC);
  String header = String("?" + portStr);
  //controls the Arduino if you press the buttons
  if (readString.indexOf(String(header + "_Off")) > 0)
  {
    changeShortCircuitState(port, ShortCircuitOff);
    ret = true;
  }
  else if (readString.indexOf(String(header + "_2s")) > 0)
  {
    changeShortCircuitState(port, ShortCircuit2s);
    ret = true;
  }
  else if (readString.indexOf(String(header + "_5s")) > 0)
  {
    changeShortCircuitState(port, ShortCircuit5s);
    ret = true;
  }
  else if (readString.indexOf(String(header + "_Perm")) > 0)
  {
    changeShortCircuitState(port, ShortCircuitPerm);
    ret = true;
  }

  return ret;
}

void processPort(SPort& port)
{
  switch (port.shortState)
  {
  case ShortCircuit2s:
    if (port.timer > TIMER_COUNT_2S)
    {
      changeRelayState(port, Off);
      changeShortCircuitState(port, ShortCircuitOff);
    }
    break;

  case ShortCircuit5s:
    if (port.timer > TIMER_COUNT_5S)
    {
      changeRelayState(port, Off);
      changeShortCircuitState(port, ShortCircuitOff);
    }
    break;

  case ShortCircuitOff:
  case ShortCircuitPerm:
  default:
    break;
  }
}

void TimerLoop()
{
  for (int i = 0; i < NUM_OF_PORTS; ++i)
  {
    if (port[i].timer) ++port[i].timer;
  }
}

void initPort(SPort& port)
{
  pinMode(port.relayPin, OUTPUT);
  pinMode(port.dcVolPin, INPUT);

  changeRelayState(port, Off);
  changeShortCircuitState(port, ShortCircuitOff);
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  for (int i = 0; i < NUM_OF_PORTS; ++i)
  {
      initPort(port[i]);
  }

  Timer1.initialize(TIMER0_PERIOD);
  Timer1.attachInterrupt(TimerLoop);
}

void loop()
{
  for (int i = 0; i < NUM_OF_PORTS; ++i)
  {
    processPort(port[i]);
  }

  // Create a client connection
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100)
        {
          //store characters to string
          readString += c;
         }

         //if HTTP request has ended
         if (c == '\n')
         {
           for (int i = 0; i < NUM_OF_PORTS; ++i)
           {
             if (httpReadButton(readString, port[i], i))
             {
               break;
             }
           }

           //clearing string for next read
           readString="";

           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println("Refresh: 5");  // refresh the page automatically every 5 sec
           client.println();
           client.println("<html>");
           client.println("<head>");
           client.println("<title>RET tester</title>");
            client.println("<style type=\"text/css\">");
            client.println("body");
            client.println("{");
            client.println("margin:60px 0px;");
            client.println("padding:0px;");
            client.println("text-align:center;");
            client.println("}");
            client.println("a");
            client.println("{");
            // client.println("text-decoration:none;");
            client.println("width:75px;");
            client.println("height:50px;");
            client.println("border-color:black;");
            client.println("border-top:2px solid;");
            client.println("border-bottom:2px solid;");
            client.println("border-right:2px solid;");
            client.println("border-left:2px solid;");
            client.println("border-radius:10px 10px 10px;");
            client.println("-o-border-radius:10px 10px 10px;");
            client.println("-webkit-border-radius:10px 10px 10px;");
            client.println("font-family:\"Trebuchet MS\",Arial, Helvetica, sans-serif;");
            client.println("-moz-border-radius:10px 10px 10px;");
            client.println("background-color:#55c7f2;");
            client.println("padding:8px;");
            client.println("text-align:center;");
            client.println("}");
            client.println("</style>");

           httpRefreshSection(client);

           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
//           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("</head>");
           client.println("<body>");
           client.println("<h1>RET tester - DC Short circuit test</h1>");
           client.println("<hr />");
           client.println("<br />");

           // section with ports information - short control buttons and dc voltage status
           for (int i = 0; i < NUM_OF_PORTS; ++i)
           {
             httpPortSection(client, port[i], i);
           }

           client.println("</body>");
           client.println("</html>");

           delay(1);
           //stopping client
           client.stop();
         }
       }
    }
  }
}
