#define SerialTxControl 10   //RS485 управляющий контакт на arduino pin 10
#define RS485Transmit    HIGH
#define RS485Receive     LOW  

char buffer[100];

void setup(void) {
  Serial.begin(9600, SERIAL_8N1);
   pinMode(SerialTxControl, OUTPUT);  
   digitalWrite(SerialTxControl, RS485Transmit); 
  // Serial.println("TEST");  
  delay(100); 
  digitalWrite(SerialTxControl, RS485Receive);   
}
 
void loop(void) {
   digitalWrite(SerialTxControl, RS485Transmit); 
   Serial.write(0x02);
   Serial.write(0x03);
   Serial.write(0x00);
   Serial.write(0x00);
   Serial.write(0x00);
   Serial.write(0x01);
   Serial.write(0x84);
   Serial.write(0x39);
// Serial.println("");
   
//   Serial.write(0x020300);
//   Serial.write(0x000001);
//   Serial.write(0x8439);
  
  digitalWrite(SerialTxControl, RS485Receive);  // читаем данные с порта
 int i=0; 
 delay(100);
 //if(Serial.available())
 {delay(100);
 
 while( i< 200)//Serial.available() && i< 99) 
 { buffer[i++] = Serial.read();} buffer[i++]='\0';}
  if(i>0)
  {
  Serial.println(buffer); // Выводим что приняли с других устройств
  }
  
  delay(3000);
 
}


