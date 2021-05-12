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
#define DATA_AV 2
#define KEY0 4
#define ADD 0xA
#define SUB 0xB
#define MUL 0xC
#define DIV 0xD
#define CLR 0xE
#define CALC 0xF

int counter = 0;
int state = 0;
int next_state = 0;
boolean s1, s2;

// fungsi untuk menampilkan BCD ke 7 segment
void writeDigit(int bcd) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BCD1 + i, bcd & 0x01);
    bcd = bcd >> 1;
  }
}

boolean keyIsPressed() {
  static boolean key_old = 0;
  boolean b1 = digitalRead(DATA_AV);
  boolean s1 = (!b1 && key_old);;
  key_old = b1;
  return s1;
}

int readKey() {
  int key = digitalRead(KEY0);
  key |= digitalRead(KEY0 + 1) << 1;
  key |= digitalRead(KEY0 + 2) << 2;
  key |= digitalRead(KEY0 + 3) << 3;
  return key;
}

char biner_table[] = {0xD, 0xE, 0, 0xF, 0xC, 9, 8, 7, 0xB, 6, 5, 4, 0xA, 3, 2, 1};
char keyToBiner(int key) {
  return biner_table[key];
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

void writeOperator(int op) {
  digits[0] = op; // op akan sesuai hexa a,b,c,d
  digits[1] = 15; // tidak tampil apa apa
  digits[2] = 15; // tidak tampil apa apa
}
#define STATE_1 0
#define STATE_2 1
boolean da;
int key, biner;
int value1, value2;
int opr;
long next_display;


void entryValue1(int biner) {
  if (value1 < 100) {
    value1 = (value1 * 10) + biner;
    writeDigit3(value1);
  }
  Serial.print(" value1= ");
  Serial.println(value1);
}

void entryValue2(int biner) {
  if (value2 < 100) {
    value2 = (value2 * 10) + biner;
    writeDigit3(value2);
  }
  Serial.print(" value2= ");
  Serial.println(value2);
}

void push(int op) {
  Serial.print(" push ");
  Serial.println(value1);
  opr = op;
  writeOperator(opr);
}

void pop() {
  Serial.print(" pop ");
  Serial.print(value1);
  Serial.print(" ");
  Serial.print(opr, HEX);
  Serial.print(" ");
  Serial.print(value2);
  switch (opr) {
    case ADD: value1 = value1 + value2; break;
    case SUB: value1 = value1 - value2; break;
    case MUL: value1 = value1 * value2; break;
    case DIV: value1 = value1 / value2; break;
  }
  value2 = 0;
  writeDigit3(value1);
  Serial.print(" => ");
  Serial.println(value1);
}

void clear() {
  value2 = value1 = 0;
  writeDigit3(value1);
  Serial.println(" clear");
}

void state1() {
  Serial.print("State 1 ");
  switch (biner) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      entryValue1(biner); break;
    case ADD:
    case SUB:
    case MUL:
    case DIV:
      push(biner);
      next_state = STATE_2;
      break;
    case CLR:
      clear();
      break;
  }
}

void state2() {
  Serial.print("State 2 ");
  switch (biner) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      entryValue2(biner); break;
    case ADD:
    case SUB:
    case MUL:
    case DIV:
      pop();
      push(biner);
      next_state = STATE_2;
      break;
    case CLR:
      clear();
      next_state = STATE_1;
      break;
    case CALC:
      pop();
      next_state = STATE_1;
      break;
  }
}


void setup() {
  Serial.begin(9600);
  // inisialisasi mode pin
  for (int pin = BCD1; pin <= T2; pin++) {
    pinMode(pin, OUTPUT);
  }
  for (int pin = DATA_AV; pin <= KEY0 + 3; pin++) {
    pinMode(pin, INPUT);
  }

  writeDigit3(0);
  digitalWrite(T1, LOW);
  digitalWrite(T2, LOW);
  next_display = millis();
}

void loop() {

  // baca semua input
  da = keyIsPressed();
  // baca waktu dalam mili detik
  long t = millis();
  if (da) {
    // baca key dan translasi ke biner
    key = readKey();
    biner = keyToBiner(key);
    // tampilkan untuk monitoring
    Serial.print("Key = ");
    Serial.print(key, BIN);
    Serial.print(" = ");
    Serial.println(biner, HEX);
    // proses sesuai state
    switch (state) {
      case STATE_1 : state1(); break;
      case STATE_2 : state2(); break;
    }
  }
  else if (t > next_display) {
    shiftDigit();
    next_display += D_PERIODE;
  }
  // update state
  if (next_state != state) {
    state = next_state;
  }
}
