#include <SPI.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Defines.h"

void changeRelayState(ERelayState newState)
{
  static ERelayState state = On;

  if (state != newState)
  {
    digitalWrite(RELAY_PIN, newState == On ? false : true);
    state = newState;
  }
}

void changeShortCircuitState(EShortCircuitState newState)
{
  shortCircuitState = newState;

  switch (shortCircuitState)
  {
    case ShortCircuit2s:
    case ShortCircuit5s:
      timerCounter = 1;
      changeRelayState(On);
      break;

    case ShortCircuitPerm:
      timerCounter = 0;
      changeRelayState(On);
      break;

    case ShortCircuitOff:
    default:
      timerCounter = 0;
      changeRelayState(Off);
      break;
  }

  addRefeshSection = true;
}

float getDcVoltage()
{
  int sensorReading = analogRead(A0);
  float dcVoltage = static_cast<float>(sensorReading)*2/100; ///1024*20;    // sensorValue/1024   - 1024=5V ->

  return dcVoltage;
}

void refreshSection(EthernetClient& client)
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

void TimerLoop()
{
  if (timerCounter) ++timerCounter;
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

  pinMode(RELAY_PIN, OUTPUT);
  changeRelayState(Off);
  changeShortCircuitState(ShortCircuitOff);

  Timer1.initialize(TIMER0_PERIOD);
  Timer1.attachInterrupt(TimerLoop);
}

void loop()
{
  switch (shortCircuitState)
  {
  case ShortCircuit2s:
    if (timerCounter > TIMER_COUNT_2S)
    {
      changeRelayState(Off);
      changeShortCircuitState(ShortCircuitOff);
    }
    break;

  case ShortCircuit5s:
    if (timerCounter > TIMER_COUNT_5S)
    {
      changeRelayState(Off);
      changeShortCircuitState(ShortCircuitOff);
    }
    break;

  case ShortCircuitOff:
  case ShortCircuitPerm:
  default:
    break;
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
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
         }

         //if HTTP request has ended
         if (c == '\n')
         {
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?buttonShortOff") >0)           changeShortCircuitState(ShortCircuitOff);
           else if (readString.indexOf("?buttonShort2s") >0)       changeShortCircuitState(ShortCircuit2s);
           else if (readString.indexOf("?buttonShort5s") >0)       changeShortCircuitState(ShortCircuit5s);
           else if (readString.indexOf("?buttonShortPerm") >0)     changeShortCircuitState(ShortCircuitPerm);

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

           refreshSection(client);
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
//           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("</head>");
           client.println("<body>");
           client.println("<h1>RS-485 - RET tester</h1>");
           client.println("<hr />");
           client.println("<br />");
           client.println("<h2>DC Short circuit test</h2>");
           client.println("<a href=\"/?buttonShortOff\"\">1. Normal</a>");
           client.println("<a href=\"/?buttonShort2s\"\">2. Short for 2s</a>");
           client.println("<a href=\"/?buttonShort5s\"\">3. Short for 5s</a>");
           client.println("<a href=\"/?buttonShortPerm\"\">4. Permanent short</a><br />");
           client.println("<br />");
           client.println("<h2>RET Status</h2>");
           // DC Voltage section
           client.print("<a style=\"background-color:#4DB257\">DC Voltage = ");  //
           client.print(getDcVoltage());
           client.println("V </a>");
           // Short circuit section
           client.print("<a style=\"background-color:#4DB257\">Short circuit mode: ");  //
           client.print(shortCircuitState);
           client.println("</a>");
           client.println("<br />");
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
