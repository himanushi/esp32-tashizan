#include <M5Unified.h>
#include <Preferences.h>

// 定数
const unsigned long TIMEOUT_DURATION = 1 * 60 * 1000;  // 3分

// グローバル変数
Preferences preferences;  // NVS操作用のオブジェクト
int num1, num2;           // 足し算の問題用の数値
int answer;               // 正解
bool showingQuestion = true;  // true: 問題表示中, false: 回答表示中
unsigned long lastButtonPress = 0;  // チャタリング防止用
unsigned long lastActivityTime = 0;  // 最後の操作時間
int maxNumber = 10;       // 乱数の最大値（初期値10）

// 新しい問題を生成
void generateNewQuestion() {
  num1 = random(1, maxNumber);  // 1からmaxNumber-1までの乱数
  num2 = random(1, maxNumber);
  answer = num1 + num2;
}

// バッテリー残量とmaxNumberを表示
void displayBatteryLevel() {
  M5.Display.setTextSize(1);
  M5.Display.setCursor(20, 110);
  M5.Display.printf("%d%%", M5.Power.getBatteryLevel());
  M5.Display.setCursor(100, 110);
  M5.Display.printf("max: %d", maxNumber);
}

// 問題を表示
void displayQuestion() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(20, 20);
  M5.Display.setTextSize(4);
  M5.Display.printf("%d + %d", num1, num2);
  M5.Display.setCursor(20, 60);
  M5.Display.printf("= ?");
  displayBatteryLevel();
}

// 答えを表示
void displayAnswer() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(20, 20);
  M5.Display.setTextSize(4);
  M5.Display.printf("%d + %d", num1, num2, answer);
  M5.Display.setCursor(20, 60);
  M5.Display.printf("= %d", answer);
  displayBatteryLevel();
}

void setup() {
  M5.begin();
  
  M5.Display.setRotation(1);  // 横向きに設定
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  
  // NVSを初期化してmaxNumberを読み込む
  preferences.begin("tashizan", false);  // falseは読み書きモード
  maxNumber = preferences.getInt("maxNum", 10);  // 保存値がない場合は10
  
  // 乱数初期化
  randomSeed(analogRead(0));
  
  // 最初の問題を生成
  generateNewQuestion();
  displayQuestion();
  
  // 最終活動時間を初期化
  lastActivityTime = millis();
}

void loop() {
  M5.update();  // ボタンの状態を更新

  // いずれかのボタンが押された場合
  if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnPWR.wasPressed()) {
    lastActivityTime = millis();  // 最終活動時間を更新
  }

  // タイムアウトチェック - 3分経過で電源オフ
  if (millis() - lastActivityTime > TIMEOUT_DURATION) {
    M5.Power.powerOff();  // 完全に電源オフ
  }

  // 上ボタン（左側）が押されたときに最大値を増やす
  if (M5.BtnB.wasPressed()) {
    maxNumber++;
    if (maxNumber > 100) maxNumber = 100;  // 最大値は100まで
    preferences.putInt("maxNum", maxNumber);  // 値を保存
    generateNewQuestion();
    displayQuestion();
  }

  // 下ボタン（右側）が押されたときに最大値を減らす
  if (M5.BtnPWR.wasPressed()) {
    maxNumber--;
    if (maxNumber < 2) maxNumber = 2;  // 最小値は2（1以上の乱数を生成するため）
    preferences.putInt("maxNum", maxNumber);  // 値を保存
    generateNewQuestion();
    displayQuestion();
  }

  // 中央ボタンが短く押されたときに答えの表示/非表示を切り替え
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
