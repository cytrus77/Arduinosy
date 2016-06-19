void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A5, INPUT);
  pinMode(5, INPUT);
}

  int okres = 1000;
  int capaTouchLast = 0;
  bool toggle = true;
  
void loop() {

  // put your main code here, to run repeatedly:
 // digitalWrite(7, HIGH);   // sets the LED on
//  delay(1000);                  // waits for a second
 // digitalWrite(7, LOW);    // sets the LED off
 // delay(okres);
  delay(50);
  int swiatlo = analogRead(A5); 
  Serial.print("Wartosc swiatla: ");
  Serial.println(swiatlo);
  analogWrite(6, swiatlo/4);
    
  int capatouch = digitalRead(5); 
  Serial.print("Wartosc capatoucha: ");
  Serial.println(capatouch);
  if (capatouch != capaTouchLast)
  {
    capaTouchLast = capatouch;

    if (capatouch)
    {
      toggle = !toggle;
      digitalWrite(7, toggle);
    }
  }
  
}
