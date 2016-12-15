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

void changeRs485Mode(ERs485State newState)
{
  rs485State = newState;
  addRefeshSection = true;
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
  float dcVoltage = static_cast<float>(sensorReading)/1024*20;    // sensorValue/1024   - 1024=5V ->

  return dcVoltage;
}

void serialWriteString()
{
  // if (sendString)
  // {
  //   char* string = msgBuffer.readString[msgBuffer.counter];
  //
  //   if (*string == 0x7F)
  //   {
  //     for (int i = 0; i < MAX_STRING_LENGTH; ++i)
  //     {
  //       Serial.write(*string);
  //       if (*string == 0x7F) break;
  //     }
  //   }
  //
  //   sendString = false;
  // }
}

void serialReadString()
{
  // static bool newString = false;
  // static char stringCounter = 0;
  //
  // if (Serial.available())
  // {
  //   char received = Serial.read();
  //
  //   if (received == 0x7F)
  //   {
  //     if (!newString)
  //     {
  //       newString = true;
  //       stringCounter = 0;
  //       if (++msgBuffer.counter == MSG_BUFFER_SIZE) msgBuffer.counter = 0;
  //     }
  //     else
  //     {
  //       msgBuffer.readString[msgBuffer.counter][stringCounter] = received;
  //       newString = false;
  //       sendString = true;
  //     }
  //   }
  //
  //   if (newString)
  //   {
  //     msgBuffer.readString[msgBuffer.counter][stringCounter++] = received;
  //     if (stringCounter >= MAX_STRING_LENGTH) newString = false;
  //   }
  // }
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
Serial.println("START 1");
  Serial.setTimeout(SERIAL_TIMEOUT);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.println("START 2");

  pinMode(RELAY_PIN, OUTPUT);
  changeRelayState(Off);
  changeShortCircuitState(ShortCircuitOff);
  changeRs485Mode(Rs485Loopback);
  Serial.println("START 3");

  Timer1.initialize(TIMER0_PERIOD);
  Timer1.attachInterrupt(TimerLoop);
  Serial.println("START END");

}

void loop()
{
  Serial.println("LOOP");

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

  switch (rs485State)
  {
    case Rs485Loopback:
      serialReadString();
      serialWriteString();
      break;

    case Rs485RxTest:
      Serial.write("~!Elo$75~");
      break;

    case Rs485TxTest:
      serialReadString();
      break;

    case Rs485NoResponse:
      serialReadString();
      break;

    case Rs485Defined:
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
           else if (readString.indexOf("?button485Defined") >0)    changeRs485Mode(Rs485Defined);
           else if (readString.indexOf("?button485Loopback") >0)   changeRs485Mode(Rs485Loopback);
           else if (readString.indexOf("?button485RxTest") >0)     changeRs485Mode(Rs485RxTest);
           else if (readString.indexOf("?button485TxTest") >0)     changeRs485Mode(Rs485TxTest);
           else if (readString.indexOf("?button485NoResponse") >0) changeRs485Mode(Rs485NoResponse);

           //clearing string for next read
           readString="";

           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println("Refresh: 5");  // refresh the page automatically every 5 sec
           client.println();
           client.println("<HTML>");
           client.println("<HEAD>");
           refreshSection(client);
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>RS-485 - RET tester</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>RS-485 - RET tester</H1>");
           client.println("<hr />");
           client.println("<br />");
           client.println("<H2>DC Short circuit test</H2>");
           client.println("<a href=\"/?buttonShortOff\"\">1. No short circuit</a>");
           client.println("<a href=\"/?buttonShort2s\"\">2. Short circuit for 2s</a>");
           client.println("<a href=\"/?buttonShort5s\"\">3. Short circuit for 5s</a>");
           client.println("<a href=\"/?buttonShortPerm\"\">4. Permanent short circuit</a><br />");
           client.println("<br />");
           client.println("<H2>RS-485 mode</H2>");
           client.println("<a href=\"/?button485Defined\"\">1. Defined response</a>");
           client.println("<a href=\"/?button485Loopback\"\">2. RS-485 Loopback</a>");
           client.println("<a href=\"/?button485RxTest\"\">3. RS-485 RX Test</a>");
           client.println("<a href=\"/?button485TxTest\"\">4. RS-485 TX Test</a>");
           client.println("<a href=\"/?button485NoResponse\"\">5. No response</a><br />");
           client.println("<br />");
           client.println("<H2>RET Status</H2>");
           // DC Voltage section
           client.print("<a style=\"background-color:#4DB257\">DC Voltage = ");  //
           client.print(getDcVoltage());
           client.println("V </a>");
           // Short circuit section
           client.print("<a style=\"background-color:#4DB257\">Short circuit mode: ");  //
           client.print(shortCircuitState);
           client.println("</a>");
           // RS485 section
           client.print("<a style=\"background-color:#4DB257\">RS-485 mode: ");  //
           client.print(rs485State);
           client.println("</a> <br />");
           client.println("<br />");
           // Last 10 received messages section
           client.print("<H2>Last ");
           client.print(MSG_BUFFER_SIZE);
           client.println(" received messages</H2>");

          //  char counter = msgBuffer.counter;
          //  for(char i = 0; i < MSG_BUFFER_SIZE; ++i)
          //  {
          //    client.print(msgBuffer.readString[counter]);
          //    client.println("<br />");
          //    if (--counter < 0) counter = MSG_BUFFER_SIZE-1;
          //  }

           client.println("<br />");
           client.println("</BODY>");
           client.println("</HTML>");

           delay(1);
           //stopping client
           client.stop();
         }
       }
    }
  }
}
