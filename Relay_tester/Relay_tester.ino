#define RELAY1PIN    2
#define RELAY2PIN    3
#define FB_RELAY1PIN 6
#define FB_RELAY2PIN 7

#define LOOPCOUNT    100


void setup() {
  Serial.begin(19200);
  Serial.println("setup()");
  
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  pinMode(FB_RELAY1PIN, INPUT);
  digitalWrite(FB_RELAY1PIN, HIGH); 
  pinMode(FB_RELAY2PIN, INPUT);
  digitalWrite(FB_RELAY2PIN, HIGH); 
}

void loop() {
  // put your main code here, to run repeatedly:

  static unsigned int count = 1;
  static unsigned int ok1 = 0;
  static unsigned int nok1 = 0;
  static unsigned int ok2 = 0;
  static unsigned int nok2 = 0;

  if(count <= LOOPCOUNT)
  {
    for (unsigned int it = 0; it < 4; ++it)
    {
      Serial.print("Proba: ");
      Serial.print(count);
      Serial.print("/");
      Serial.print(LOOPCOUNT);
      Serial.println("");
      digitalWrite(RELAY1PIN, it & 0x1);
      digitalWrite(RELAY2PIN, it & 0x2);
      delay(100);
      if (digitalRead(FB_RELAY1PIN) == ((it & 0x1) ? HIGH : LOW))
      {
        ++ok1;
        Serial.print("Rel1 - OK ; ");
      }
      else
      {
        ++nok1;
        Serial.print("Rel1 - NOK !!! ; ");
      }

      if (digitalRead(FB_RELAY2PIN) == ((it & 0x2) ? HIGH : LOW))
      {
        ++ok2;
        Serial.println("Rel2 - OK");
      }
      else
      {
        ++nok2;
        Serial.println("Rel2 - NOK !!!");
      }
      delay(1000);
      if (digitalRead(FB_RELAY1PIN) == ((it & 0x1) ? HIGH : LOW))
      {
        ++ok1;
        Serial.print("Rel1 - OK ; ");
      }
      else
      {
        ++nok1;
        Serial.print("Rel1 - NOK !!! ; ");
      }

      if (digitalRead(FB_RELAY2PIN) == ((it & 0x2) ? HIGH : LOW))
      {
        ++ok2;
        Serial.println("Rel2 - OK ");
      }
      else
      {
        ++nok2;
        Serial.println("Rel2 - NOK !!! ");
      }
    }

    ++count;
  }
  else if(count == (LOOPCOUNT+1))
  {
    Serial.println("Podsumowanie testu");
    Serial.print("Wykonano prob: ");
    Serial.println(count-1);
    Serial.print("Przekaznik 1 - Udanych: ");
    Serial.println(ok1);
    Serial.print("Przekaznik 1 - Nie udanych: ");
    Serial.println(nok1);
    Serial.print("Przekaznik 2 - Udanych: ");
    Serial.println(ok2);
    Serial.print("Przekaznik 2 - Nie udanych: ");
    Serial.println(nok2);
    unsigned long duration = millis();
    unsigned long minutes = duration / 60000;
    duration = duration - (minutes*60000);
    Serial.print("Czas trwania testu: ");
    Serial.print(minutes);
    Serial.print(" minut ");
    Serial.print(duration/1000);
    Serial.print(".");
    Serial.print(duration%1000);
    Serial.println(" sekund");
    ++count;
  }
}
