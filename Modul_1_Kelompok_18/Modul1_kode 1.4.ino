#define BUTTON1 4
#define BUTTON2 2
#define BCD1 8
#define T1 12
#define T2 13
#define STATE_UP 0
#define STATE_DOWN 1

// fungsi untuk menampilkan BCD ke 7 segment
void writeDigit(int bcd) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BCD1 + i, bcd & 0x01);
    bcd = bcd >> 1;
  }
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
void setup() {
  Serial.begin(9600);
  // inisialisasi mode pin
  for (int pin = BCD1; pin <= T2; pin++) {
    pinMode(pin, OUTPUT);
    // siapkan angka 123
    writeDigit3(123);
    // Inisialisasi decoder
    digitalWrite(T1, LOW);
    digitalWrite(T2, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // program menggeser nyala LED
  boolean s1 = digitalRead(BUTTON1);
  boolean s2 = digitalRead(BUTTON2);
  shiftDigit();
  delay(DISPLAY_PERIODE);
  if (s1) {
    DISPLAY_PERIODE = DISPLAY_PERIODE - 1;
  };
  if (!s2) {
    DISPLAY_PERIODE = DISPLAY_PERIODE - 1; delay(500);
  };
  Serial.println(DISPLAY_PERIODE);
  if (DISPLAY_PERIODE == 0) {
    DISPLAY_PERIODE = 100;
  };
}
