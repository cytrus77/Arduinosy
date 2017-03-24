#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "TimerOne.h"

#define MAX_HDLC_FRAME_SIZE  256
#define HDLC_SS_BYTE         0x7E
#define TIMER_PERIOD         300000 // in us

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xD1, 0xD1, 0xBE, 0x2A, 0xFA, 0x2A };
IPAddress ip(192, 168, 0, 143);
unsigned int localPort = 13201;      // local port to listen on
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
char udpBuffer[MAX_HDLC_FRAME_SIZE]; //buffer to hold incoming packet,
char uartBuffer[MAX_HDLC_FRAME_SIZE];
bool hdlcStart = false;
unsigned int uartCounter = 0;

void resetUartBuffer()
{
  hdlcStart = false;
  uartCounter = 0;
}

void udpToUart()
{
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    // read the packet into packetBufffer
    Udp.read(udpBuffer,MAX_HDLC_FRAME_SIZE);
    Serial.write(HDLC_SS_BYTE);
    Serial.write(udpBuffer, packetSize);
    Serial.write(0x12);
    Serial.write(0x34);
    Serial.write(HDLC_SS_BYTE);
  }
}

void uartToUdp()
{
  bool sendIt = false;

  if (Serial.available() > 0)
  {
    uartBuffer[uartCounter] = Serial.read();
    if (uartBuffer[uartCounter] == HDLC_SS_BYTE)
    {
      if (hdlcStart) sendIt = true;
      else hdlcStart = true;
    }
    else if (hdlcStart)
    {
      ++uartCounter;
    }
  }

  if (sendIt)
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(uartBuffer, uartCounter);
    Udp.endPacket();
    resetUartBuffer();
  }
}

void TimerLoop()
{
  resetUartBuffer();
}

void setup()
{
  Serial.begin(9600);
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);

  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(TimerLoop);
}

void loop()
{
  udpToUart();
  uartToUdp();
}
