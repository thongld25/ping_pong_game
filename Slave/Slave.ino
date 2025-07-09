#define P2_UP 14
#define P2_DOWN 27

// UART2: RX = 16, TX = 17 (giống Master nhưng ngược chiều)
#define RXD2 16
#define TXD2 17

unsigned long lastSend = 0;

void setup() {
  pinMode(P2_UP, INPUT_PULLUP);
  pinMode(P2_DOWN, INPUT_PULLUP);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // Đọc nút UP
  if (!digitalRead(P2_UP)) {
    Serial2.println("UP");
    delay(50);
  }

  // Đọc nút DOWN
  if (!digitalRead(P2_DOWN)) {
    Serial2.println("DOWN");
    delay(50);
  }

  delay(10);
}
