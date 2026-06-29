#define LED_A_PIN 4
#define LED_B_PIN 16

const int freq = 5000;       // PWM频率
const int resolution = 8;    // 8位分辨率，占空比范围0~255


void setup() {
  Serial.begin(115200);

  // 初始化两个独立PWM输出
  ledcAttach(LED_A_PIN, freq, resolution);
  ledcAttach(LED_B_PIN, freq, resolution);

  Serial.println("Two LED inverse PWM start");
}

void loop() {

  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(LED_A_PIN, dutyCycle);
    ledcWrite(LED_B_PIN, 255 - dutyCycle);

    delay(10);
  }


  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    ledcWrite(LED_A_PIN, dutyCycle);
    ledcWrite(LED_B_PIN, 255 - dutyCycle);

    delay(10);
  }
}