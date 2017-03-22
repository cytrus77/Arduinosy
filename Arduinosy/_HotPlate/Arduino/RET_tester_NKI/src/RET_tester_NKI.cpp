#include <SPI.h>
#include <string.h>
#include "TimerOne.h"
#include "Defines.h"

void setRs485Direction(bool direction)
{
  digitalWrite(RS485_RX_DIRECTION_PIN, direction);
  digitalWrite(RS485_TX_DIRECTION_PIN, direction);
}

void changeRs485Mode(ERs485State newState)
{
  rs485State = newState;
  addRefeshSection = true;

  if (Rs485RxTest == rs485State)
  {
    setRs485Direction(RS485Transmit);
  }
  else
  {
    setRs485Direction(RS485Receive);
  }
}

void serialWriteString()
{
  if (sendString)
  {
    setRs485Direction(RS485Transmit);
    Serial.print(msgBuffer.readString[msgBuffer.counter]);
    sendString = false;
    setRs485Direction(RS485Receive);
  }
}

void serialReadString()
{
  static bool newString = false;
  static char stringCounter = 0;

  while (Serial.available())
  {
    char received = Serial.read();

    if (received == 0x7E)
    {
      if (!newString)
      {
        newString = true;
        stringCounter = 0;
        if (++msgBuffer.counter == MSG_BUFFER_SIZE) msgBuffer.counter = 0;
      }
      else
      {
        msgBuffer.readString[msgBuffer.counter][stringCounter++] = received;
        newString = false;
        sendString = true;

        while (stringCounter < MAX_STRING_LENGTH)
        {
          msgBuffer.readString[msgBuffer.counter][stringCounter++] = 0;
        }

        break;
      }
    }

    if (newString)
    {
      msgBuffer.readString[msgBuffer.counter][stringCounter++] = received;

      if (stringCounter == (MAX_STRING_LENGTH-1))
      {
        msgBuffer.readString[msgBuffer.counter][stringCounter] = 0x7E;
        newString = false;
        sendString = true;
        break;
      }
    }
  }
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
  ++timerCounter;
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.setTimeout(SERIAL_TIMEOUT);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  Timer1.initialize(TIMER0_PERIOD);
  Timer1.attachInterrupt(TimerLoop);

  pinMode(RS485_RX_DIRECTION_PIN, OUTPUT);
  pinMode(RS485_TX_DIRECTION_PIN, OUTPUT);
  setRs485Direction(RS485Receive);

  changeRs485Mode(Rs485Loopback);
}

void loop()
{
  switch (rs485State)
  {
    case Rs485Loopback:
      serialReadString();
      serialWriteString();
      break;

    case Rs485RxTest:
      if (TIMER_COUNT_1S < timerCounter)
      {
        Serial.write("~!EloHello$75~");
        timerCounter = 0;
      }
      break;

    case Rs485TxTest:
      serialReadString();
      break;

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
           if (readString.indexOf("?button485Loopback") >0)        changeRs485Mode(Rs485Loopback);
           else if (readString.indexOf("?button485RxTest") >0)     changeRs485Mode(Rs485RxTest);
           else if (readString.indexOf("?button485TxTest") >0)     changeRs485Mode(Rs485TxTest);

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
           client.println("<H2>RS-485 mode</H2>");
           client.println("<a href=\"/?button485Loopback\"\">1. RS-485 Loopback</a>");
           client.println("<a href=\"/?button485RxTest\"\">2. RS-485 RX Test</a>");
           client.println("<a href=\"/?button485TxTest\"\">3. RS-485 TX Test</a>");
           client.println("<br />");
           client.println("<H2>RET Status</H2>");
           // DC Voltage section
           // RS485 section
           client.print("<a style=\"background-color:#4DB257\">RS-485 mode: ");  //
           client.print(rs485State);
           client.println("</a> <br />");
           client.println("<br />");
           // Last 10 received messages section
           client.print("<H2>Last ");
           client.print(MSG_BUFFER_SIZE);
           client.println(" received messages</H2>");

           char counter = msgBuffer.counter;
           for(char i = 0; i < MSG_BUFFER_SIZE; ++i)
           {
             client.print(msgBuffer.readString[counter]);
             client.println("<br />");
             if (--counter < 0) counter = MSG_BUFFER_SIZE-1;
           }

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
