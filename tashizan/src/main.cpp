#include <M5Unified.h>
#include <Preferences.h>

// 定数
const unsigned long TIMEOUT_DURATION = 5 * 60 * 1000;  // 5分

// 計算の種類を定義
enum CalcType {
  ADD = 0,    // 足し算
  SUB = 1,    // 引き算
  MUL = 2,    // 掛け算
  DIV = 3     // 割り算
};

// グローバル変数
Preferences preferences;  // NVS操作用のオブジェクト
int num1, num2;           // 計算用の数値
int answer;               // 正解
bool showingQuestion = true;  // true: 問題表示中, false: 回答表示中
unsigned long lastButtonPress = 0;  // チャタリング防止用
unsigned long lastActivityTime = 0;  // 最後の操作時間
CalcType currentCalc = ADD;  // 現在の計算種類
int maxNumbers[4] = {10, 10, 10, 10};  // 各計算種類ごとの最大値

// 計算記号を取得
const char* getCalcSymbol() {
  switch (currentCalc) {
    case ADD: return "+";
    case SUB: return "-";
    case MUL: return "x";
    case DIV: return "/";
    default: return "+";
  }
}

// 計算種類の名前を取得
const char* getCalcName() {
  switch (currentCalc) {
    case ADD: return "ADD";
    case SUB: return "SUB";
    case MUL: return "MUL";
    case DIV: return "DIV";
    default: return "ADD";
  }
}

// 新しい問題を生成
void generateNewQuestion() {
  int maxNumber = maxNumbers[currentCalc];
  
  switch (currentCalc) {
    case ADD:
      num1 = random(1, maxNumber);
      num2 = random(1, maxNumber);
      answer = num1 + num2;
      break;
      
    case SUB:
      num1 = random(1, maxNumber);
      num2 = random(1, num1);  // num1より小さい数を生成
      answer = num1 - num2;
      break;
      
    case MUL:
      num1 = random(1, maxNumber);
      num2 = random(1, maxNumber);
      answer = num1 * num2;
      break;
      
    case DIV:
      num2 = random(1, maxNumber);  // 除数を先に決める
      answer = random(1, maxNumber);  // 商を決める
      num1 = num2 * answer;  // 被除数を計算（必ず割り切れる）
      break;
  }
}

// バッテリー残量とmaxNumberを表示
void displayBatteryLevel() {
  M5.Display.setTextSize(1);
  M5.Display.setCursor(20, 110);
  M5.Display.printf("%d%%", M5.Power.getBatteryLevel());
  M5.Display.setCursor(100, 110);
  M5.Display.printf("%s max: %d", getCalcName(), maxNumbers[currentCalc]);
}

// 問題を表示
void displayQuestion() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(20, 20);
  M5.Display.setTextSize(4);
  M5.Display.printf("%d %s %d", num1, getCalcSymbol(), num2);
  M5.Display.setCursor(20, 60);
  M5.Display.printf("= ?");
  displayBatteryLevel();
}

// 答えを表示
void displayAnswer() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(20, 20);
  M5.Display.setTextSize(4);
  M5.Display.printf("%d %s %d", num1, getCalcSymbol(), num2);
  M5.Display.setCursor(20, 60);
  M5.Display.printf("= %d", answer);
  displayBatteryLevel();
}

void setup() {
  M5.begin();
  
  M5.Display.setRotation(1);  // 横向きに設定
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  
  // NVSを初期化して各計算種類のmaxNumberを読み込む
  preferences.begin("tashizan", false);  // falseは読み書きモード
  maxNumbers[ADD] = preferences.getInt("maxAdd", 10);
  maxNumbers[SUB] = preferences.getInt("maxSub", 10);
  maxNumbers[MUL] = preferences.getInt("maxMul", 10);
  maxNumbers[DIV] = preferences.getInt("maxDiv", 10);
  
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

  // タイムアウトチェック - 5分経過で電源オフ
  if (millis() - lastActivityTime > TIMEOUT_DURATION) {
    M5.Power.powerOff();  // 完全に電源オフ
  }

  // 決定ボタンが押されている状態での上下ボタン操作
  if (M5.BtnA.isPressed()) {
    // 上ボタンでレベルを上げる
    if (M5.BtnB.wasPressed()) {
      maxNumbers[currentCalc]++;
      if (maxNumbers[currentCalc] > 100) maxNumbers[currentCalc] = 100;
      // 計算種類に応じた保存キーで値を保存
      switch (currentCalc) {
        case ADD: preferences.putInt("maxAdd", maxNumbers[currentCalc]); break;
        case SUB: preferences.putInt("maxSub", maxNumbers[currentCalc]); break;
        case MUL: preferences.putInt("maxMul", maxNumbers[currentCalc]); break;
        case DIV: preferences.putInt("maxDiv", maxNumbers[currentCalc]); break;
      }
      generateNewQuestion();
      displayQuestion();
    }
    
    // 下ボタンでレベルを下げる
    if (M5.BtnPWR.wasPressed()) {
      maxNumbers[currentCalc]--;
      if (maxNumbers[currentCalc] < 2) maxNumbers[currentCalc] = 2;
      // 計算種類に応じた保存キーで値を保存
      switch (currentCalc) {
        case ADD: preferences.putInt("maxAdd", maxNumbers[currentCalc]); break;
        case SUB: preferences.putInt("maxSub", maxNumbers[currentCalc]); break;
        case MUL: preferences.putInt("maxMul", maxNumbers[currentCalc]); break;
        case DIV: preferences.putInt("maxDiv", maxNumbers[currentCalc]); break;
      }
      generateNewQuestion();
      displayQuestion();
    }
  } else {
    // 決定ボタンが押されていない状態での上下ボタン操作
    
    // 上ボタンで計算種類を切り替え（増加）
    if (M5.BtnB.wasPressed()) {
      currentCalc = static_cast<CalcType>((currentCalc + 1) % 4);
      generateNewQuestion();
      displayQuestion();
    }
    
    // 下ボタンで計算種類を切り替え（減少）
    if (M5.BtnPWR.wasPressed()) {
      currentCalc = static_cast<CalcType>((currentCalc + 3) % 4);  // +3は-1と同じ
      generateNewQuestion();
      displayQuestion();
    }
  }

  // 決定ボタンの短押しで答えの表示/非表示を切り替え
  if (M5.BtnA.wasPressed()) {
    unsigned long currentTime = millis();
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
