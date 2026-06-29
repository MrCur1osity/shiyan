#define TOUCH_PIN 4
#define LED_PIN   2
#define THRESHOLD 100       // 需根据实际触摸值调整

bool ledState = false;          // LED当前状态
bool lastTouchstate = false;   //松手检测
unsigned long lastTriggerTime = 0;

const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)
int dutyCycle = 0;              // 当前亮度
int fadeDirection = 1;          // 1：渐亮；-1：渐暗
unsigned long lastFadeTime = 0;
int speed=1;
const int delaytime[3]={1,10,20};
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  ledcAttach(LED_PIN, freq, resolution);
  Serial.print("Speed Level: 1");
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < THRESHOLD);
  // 边缘检测：
  // 上一次未触摸 && 当前触摸 → 判定为一次“按下瞬间”
  if (!lastTouchstate&&currentTouchState) {
    // 软件防抖
    if (millis() - lastTriggerTime > 150) {
      speed=speed%3+1;
      Serial.print("Speed Level: ");
      Serial.println(speed);
      lastTriggerTime = millis();

    }
  }
   lastTouchstate=currentTouchState;
   // 非阻塞式呼吸灯：不会卡住触摸检测
  if (millis() - lastFadeTime >= delaytime[speed - 1]) {
    lastFadeTime = millis();

    dutyCycle += fadeDirection;

    // 达到最亮后开始变暗
    if (dutyCycle >= 255) {
      dutyCycle = 255;
      fadeDirection = -1;
    }

    // 达到最暗后开始变亮
    if (dutyCycle <= 0) {
      dutyCycle = 0;
      fadeDirection = 1;
    }

    ledcWrite(LED_PIN, dutyCycle);
  } 
}