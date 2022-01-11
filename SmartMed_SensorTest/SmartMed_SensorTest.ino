const int LDR = A0;
#define pinBuzz 16

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pinBuzz, OUTPUT);
}

int feq = 0;

void loop() {
  while (Serial.available()) {        // If HC-12 has data
    char c = Serial.read();
    if(c == 'u') feq = 1000;
    if(c == 'd') feq = 0;
  }
  Serial.print("Ldr = ");
  Serial.println(analogRead(LDR));
  Serial.print("feq = ");
  Serial.println(feq);
  
  tone(pinBuzz, feq);
  delay(100);
  tone(pinBuzz, 0);
  delay(100);
  tone(pinBuzz, feq);
  delay(100);
  tone(pinBuzz, 0);
  
  delay(700);
}
