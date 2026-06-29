#define TOUCH_PIN 4
#define LED_PIN   2
#define THRESHOLD 100       // 需根据实际触摸值调整

bool ledState = false;          // LED当前状态
bool lastTouchstate = false;   //松手检测
unsigned long lastTriggerTime = 0;
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < THRESHOLD);

  // 边缘检测：
  // 上一次未触摸 && 当前触摸 → 判定为一次“按下瞬间”
  if (!lastTouchstate&&currentTouchState) {
    // 软件防抖
    if (millis() - lastTriggerTime > 150) {
      ledState = !ledState;                  // 翻转LED状态
      digitalWrite(LED_PIN, ledState);

      lastTriggerTime = millis();

      if (ledState) {
        Serial.println("LED ON");
      } else {
        Serial.println("LED OFF");
      }
    }
  }

  lastTouchstate=currentTouchState;
  delay(100);   
}