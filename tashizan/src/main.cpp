#include <M5Unified.h>

// グローバル変数
int num1, num2;           // 足し算の問題用の数値
int answer;               // 正解
bool showingQuestion = true;  // true: 問題表示中, false: 回答表示中
unsigned long lastButtonPress = 0;  // チャタリング防止用

// 新しい問題を生成
void generateNewQuestion() {
  num1 = random(1, 10);
  num2 = random(1, 10);
  answer = num1 + num2;
}

// 問題を表示
void displayQuestion() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(10, 20);
  M5.Display.setTextSize(3);
  M5.Display.printf("%d + %d = ?", num1, num2);
}

// 答えを表示
void displayAnswer() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(10, 20);
  M5.Display.setTextSize(3);
  M5.Display.printf("%d + %d = %d", num1, num2, answer);
}

void setup() {
  M5.begin();
  
  M5.Display.setRotation(1);  // 横向きに設定
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  
  // 乱数初期化
  randomSeed(analogRead(0));
  
  // 最初の問題を生成
  generateNewQuestion();
  displayQuestion();
}

void loop() {
  M5.update();  // ボタンの状態を更新

  // ボタンAが押されたとき（チャタリング防止付き）
  if (M5.BtnA.wasPressed()) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > 300) {  // 300ms以上間隔が空いているか確認
      if (showingQuestion) {
        // 問題表示中なら答えを表示
        displayAnswer();
      } else {
        // 答え表示中なら新しい問題を生成して表示
        generateNewQuestion();
        displayQuestion();
      }
      showingQuestion = !showingQuestion;  // 表示状態を切り替え
      lastButtonPress = currentTime;
    }
  }
}
