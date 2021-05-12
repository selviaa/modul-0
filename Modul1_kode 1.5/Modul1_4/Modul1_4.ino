#define BUTTON1 4
#define BUTTON2 2
#define BCD1 8
#define T1 12
#define T2 13
#define STATE_UP 0
#define STATE_DOWN 1
#define STATE_IDLE 0
#define STATE_RUN 1
#define CLOCK_PERIODE 100
#define D_PERIODE 7
int counter = 0;
int state = 0;
int next_state = 0;
boolean s1, s2;
long next_clock, next_display;

// fungsi untuk menampilkan BCD ke 7 segment
void writeDigit(int bcd) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BCD1 + i, bcd & 0x01);
    bcd = bcd >> 1;
  }
}

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
  Serial.println(s2);
  return s2;
}

// fungsi memecah angka (0 999) menjadi 3 BCD ke array digits
// digits[0] = satuan, digits[1] = puluhan, digits[2] = ratusan
int digits[3];
void writeDigit3(int data) {
  digits[0] = data % 10;
  data = data / 10;
  digits[1] = data % 10;
  data = data / 10;
  digits[2] = data % 10;
}

// fungsi untuk menampilkan digits[] ke 7 segment dengan decoder
int index = 0;
void shiftDigit() {
  writeDigit(digits[index]); // memasukan nilai pada salah satu digits
  digitalWrite(T1, index & 0x02);
  digitalWrite(T2, index & 0x01);
  index = (index + 1 ) % 3;
}
int DISPLAY_PERIODE = 100;

void eventS1() {
  //Serial.print("Event S1 state ");
  //Serial.println(state);
  switch (state) {
    case STATE_IDLE :
      // ubah state
      next_state = STATE_RUN;
      break;
    case STATE_RUN:
      next_state = STATE_IDLE;
  }
}

void eventS2() {
  //Serial.print("Event S2 state ");
  //Serial.println(state);
  switch (state) {
    case STATE_IDLE: counter = 0; writeDigit3(counter); break;
    case STATE_RUN:
      // reset counter
      counter = 0;
      writeDigit3(counter); break;
  }
}

void eventClock() {
  //Serial.print("Event Clock state ");
  //Serial.println(state);
  switch (state) {
    case STATE_IDLE: break;
    case STATE_RUN:
      // naikkan counter 0 999
      counter = (counter + 1) % 1000;
      writeDigit3(counter);
  }
}

void setup() {
  Serial.begin(9600);
  // inisialisasi mode pin
  for (int pin = BCD1; pin <= T2; pin++) {
    pinMode(pin, OUTPUT);

    writeDigit3(0);
    digitalWrite(T1, LOW);
    digitalWrite(T2, LOW);
    next_clock = next_display = millis();
  }
}

void loop() {
  // baca semua input
  s1 = s1IsPressed();
  s2 = s2IsPressed();
  // baca waktu dalam mili detik
  long t = millis();
  // proses event
  // yang prioritasnya tinggi didahulukan
  if (t >= next_clock) {
    eventClock();
    next_clock = next_clock + CLOCK_PERIODE; // waktu aksi berikutnya
  }
  else if (s1) {
    eventS1();
  }
  else if (s2) {
    eventS2();
  }
  else if (t >= next_display) {
    shiftDigit();
    next_display = next_display + D_PERIODE; // waktu aksi berikutnya
  }
  if (next_state != state) {
    Serial.print("State ");
    Serial.println(state);
    Serial.print(" ==> ");
    Serial.println(next_state);
    state = next_state;
  }
}
