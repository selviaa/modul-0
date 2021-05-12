#define BUTTON1 4
#define BUTTON2 2
#define BCD1 8
#define T1 12
#define T2 13
#define STATE_UP 0
#define STATE_DOWN 1
int counter = 0;
int state, next_state;
boolean s1, s2;
byte index = 0;

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

void state0() {
  if (s1) {
    Serial.println("State UP Event S1");
    counter = (counter + 1) % 10;
    writeDigit(counter);
  }
  else if (s2) {
    Serial.println("State UP Event S2");
    next_state = STATE_DOWN;
  }
}


void state1() {
  if (s1) {
    Serial.println("State DOWN Event S1");
    counter = counter - 1;
    if (counter == -1) {
      counter = 9;
    };
    writeDigit(counter);
  }
  else if (s2) {
    Serial.println("State DOWN Event S2");
    next_state = STATE_UP;
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  for (int pin = BCD1; pin <= T2; pin++) {
    pinMode(pin, OUTPUT);
  }
  next_state = state = 0;
  counter = 0;
  writeDigit(counter);


  digitalWrite(T1, index & 0x02);
  digitalWrite(T2, index & 0x02);
}

void loop() {
  // put your main code here, to run repeatedly:
  s1 = s1IsPressed();
  s2 = s2IsPressed();
  switch (state) {
    case 0 : state0(); break;
    case 1 : state1(); break;
  }

  if (next_state != state) {
    Serial.print("State ");
    Serial.println(state);
    Serial.print(" ==> ");
    Serial.println(next_state);
    state = next_state;
  }

}
