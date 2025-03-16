#include <M5Unified.h>

// グローバル変数
int num1, num2;           // 足し算の問題用の数値
int answer;               // 正解
bool showingQuestion = true;  // true: 問題表示中, false: 回答表示中
unsigned long lastButtonPress = 0;  // チャタリング防止用
unsigned long lastActivityTime = 0;  // 最後のアクティビティ時間
const unsigned long AUTO_POWER_OFF_TIME = 5 * 60 * 1000;  // 5分間（ミリ秒）

// 新しい問題を生成
void generateNewQuestion() {
  num1 = random(1, 100);  // 1-99のランダムな数
  num2 = random(1, 100);
  answer = num1 + num2;
}

// 問題を表示
void displayQuestion() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(10, 20);
  M5.Display.setTextSize(2);
  M5.Display.printf("%d + %d = ?", num1, num2);
}

// 答えを表示
void displayAnswer() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(10, 20);
  M5.Display.setTextSize(2);
  M5.Display.printf("%d + %d = %d", num1, num2, answer);
}

// 電源管理
void checkAutoPowerOff() {
  if (millis() - lastActivityTime >= AUTO_POWER_OFF_TIME) {
    M5.Power.powerOff();
  }
}

// アクティビティ時間を更新
void updateActivityTime() {
  lastActivityTime = millis();
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // 初期アクティビティ時間を設定
  lastActivityTime = millis();
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
  
  // 自動電源オフのチェック
  checkAutoPowerOff();
  
  // 中央ボタン（電源ボタン）が押された時
  if (M5.BtnB.wasPressed()) {
    updateActivityTime();  // アクティビティ時間を更新
  }

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
      updateActivityTime();  // アクティビティ時間を更新
    }
  }
}
