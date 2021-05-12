#define BUTTON1 4
#define BUTTON2 2
#define BCD1 8
#define T1 12
#define T2 13

boolean s1IsPressed() {
  static boolean b1_old = 0;
  boolean b1 = digitalRead(BUTTON1);
  boolean s1 = (b1 && !b1_old);
  b1_old = b1;
  return s1;
}

boolean s2IsPressed() {
  static boolean b2_old = 0;
  boolean b2 = !digitalRead(BUTTON2);
  boolean s2 = (b2 && !b2_old);
  b2_old = b2;
  return s2;
}

void writeDigit(int bcd) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BCD1 + i, bcd & 0x01);
    bcd = bcd >> 1;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  for (int pin = BCD1; pin <= T2; pin++) {
    pinMode(pin, OUTPUT);
  }
  digitalWrite(T1,HIGH);
  digitalWrite(T2,HIGH);
}

void loop() {
for(int i = 0; i<16; i++){
  writeDigit(i);
  delay(1000);
  }
}
