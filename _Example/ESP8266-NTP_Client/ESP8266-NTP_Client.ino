const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

unsigned long GetTime()
{
  String cmd = "AT+CIPSTART=\"UDP\",\"192.168.0.28\",123"; // my own NTP server, synced by GPS
  Serial1.println(cmd);
  delay(2000);
  if(Serial1.find("Error")){
    Serial.print("RECEIVED: Error");
    return 0;
  }
  
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  
  Serial1.print("AT+CIPSEND=");
  Serial1.println(NTP_PACKET_SIZE);
  if(Serial1.find(">"))
  {
    for (byte i = 0; i < NTP_PACKET_SIZE; i++)
    {
      Serial1.write(packetBuffer[i]);
      delay(5);
    }
  }else{
    Serial1.println("AT+CIPCLOSE");
    return 0;
  }
  
  //Serial1.find("+IPD,48:");
  
  int acksize = NTP_PACKET_SIZE + 1 + 2 + 8; // ESP8266 adds a space, a CRLF and starts with "+IPD,48:"
  
  Serial.println("ESP2866 ACK : ");
  for (byte i = 0; i < acksize; i++)
  {
    while (Serial1.available() == 0) // you may have to wait for some bytes
    {
      //Serial.print(".");
    }
    byte ch = Serial1.read();
    if (ch < 0x10) Serial.print('0');
    Serial.print(ch,HEX);
    Serial.print(' ');
    if ( (((i+1) % 15) == 0) ) { Serial.println(); }
  }
  Serial.println();
  Serial.println();

  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  Serial.println("Server answer : ");

  int i = 0;
  while (Serial1.available() > 0) {
    byte ch = Serial1.read();
    if (i <= NTP_PACKET_SIZE)
    { 
      packetBuffer[i] = ch;
    }
    if (ch < 0x10) Serial.print('0');
    Serial.print(ch,HEX);
    Serial.print(' ');
    if ( (((i+1) % 15) == 0) ) { Serial.println(); }
    delay(5);
    i++;
    if ( ( i < NTP_PACKET_SIZE ) && ( Serial1.available() == 0 ) )
    {
      while (Serial1.available() == 0)  // you may have to wait for some bytes
      {
        Serial.print("!");
      }
    }
  }
  
  Serial.println();
  Serial.println();
  Serial.print(i+1);
  Serial.println(" bytes received"); // will be more than 48
  
  Serial.print(packetBuffer[40],HEX);
  Serial.print(" ");
  Serial.print(packetBuffer[41],HEX);
  Serial.print(" ");
  Serial.print(packetBuffer[42],HEX);
  Serial.print(" ");
  Serial.print(packetBuffer[43],HEX);
  Serial.print(" = ");

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  
  Serial.print(secsSince1900,DEC);
  
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears  = 2208988800UL;
    // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  
  unsigned long DST = 60*60*2; // adjust to your GMT+DST
  
  unsigned long timestamp = epoch + DST;

  Serial.println();
  Serial.print("Epoch : ");
  Serial.println(epoch,DEC);        

  return timestamp;
}
