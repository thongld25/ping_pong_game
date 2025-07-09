#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//SCL: D22, SDA: D21
// Nút nhấn
#define P1_UP 12
#define P1_DOWN 13

// UART
#define RXD2 16
#define TXD2 17

// Buzzer
#define BUZZER_PIN 4 //chan duong buzzer dau D4

// Trạng thái game
bool gameStarted = false;
bool gameOver = false;
bool waitingForRestart = false;

int scoreP1 = 0;
int scoreP2 = 0;

// Paddle & bóng
int paddle1Y = 20;
int paddle2Y = 20;
int ballX = 64, ballY = 32;
int ballDX = 2, ballDY = 1;

void setup() {
  pinMode(P1_UP, INPUT_PULLUP);
  pinMode(P1_DOWN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showStartScreen();
}

void loop() {
  if (!gameStarted) {
    if (!digitalRead(P1_UP) || !digitalRead(P1_DOWN) || Serial2.available()) {
      gameStarted = true;
      resetGame();
    }
    return;
  }

  if (waitingForRestart) {
    if (!digitalRead(P1_UP) || !digitalRead(P1_DOWN) || Serial2.available()) {
      waitingForRestart = false;
      resetGame();
    }
    return;
  }

  // Điều khiển Player 1
  if (!digitalRead(P1_UP)) paddle1Y -= 2;
  if (!digitalRead(P1_DOWN)) paddle1Y += 2;
  paddle1Y = constrain(paddle1Y, 0, SCREEN_HEIGHT - 16);

  // Điều khiển Player 2
  if (Serial2.available()) {
    String cmd = Serial2.readStringUntil('\n');
    cmd.trim();
    if (cmd == "UP") paddle2Y -= 2;
    else if (cmd == "DOWN") paddle2Y += 2;
    paddle2Y = constrain(paddle2Y, 0, SCREEN_HEIGHT - 16);
  }

  if (!gameOver) {
    // Cập nhật bóng
    ballX += ballDX;
    ballY += ballDY;

    if (ballY <= 0 || ballY >= SCREEN_HEIGHT) ballDY *= -1;

    if (ballX <= 6 && ballY >= paddle1Y && ballY <= paddle1Y + 16) ballDX *= -1;
    if (ballX >= 122 && ballY >= paddle2Y && ballY <= paddle2Y + 16) ballDX *= -1;

    if (ballX <= 0) {
      scoreP2++;
      endGame(2);
    } else if (ballX >= SCREEN_WIDTH) {
      scoreP1++;
      endGame(1);
    }
  }

  drawGame();
  delay(30);
}


void showStartScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 25);
  display.print("Start");
  display.display();
}

void resetGame() {
  paddle1Y = 20;
  paddle2Y = 20;
  ballX = 64;
  ballY = 32;
  ballDX = 2;
  ballDY = 1;
  gameOver = false;
  digitalWrite(BUZZER_PIN, LOW);
}

void endGame(int winner) {
  gameOver = true;

  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 20);
  if (winner == 1)
    display.print("P1 Win!");
  else
    display.print("P2 Win!");
  display.display();

  delay(2000);

  // Hiển thị lại màn hình chờ bắt đầu ván mới
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(15, 25);
  display.print("Restart");
  display.display();

  waitingForRestart = true;
}


void drawGame() {
  display.clearDisplay();

  // Vợt
  display.fillRect(0, paddle1Y, 4, 16, SSD1306_WHITE);
  display.fillRect(124, paddle2Y, 4, 16, SSD1306_WHITE);

  // Bóng
  display.fillCircle(ballX, ballY, 2, SSD1306_WHITE);

  // Điểm số
  display.setTextSize(1);
  display.setCursor(5, 0);
  display.print(scoreP1);
  display.setCursor(118, 0);
  display.print(scoreP2);

  display.display();
}
