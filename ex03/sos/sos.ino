const int ledPin = 2;

const unsigned long shortFlash = 200;
const unsigned long longFlash  = 600;
const unsigned long innerGap   = 200;
const unsigned long letterGap  = 600;
const unsigned long sosGap     = 2000;

int currentStep = 0;
unsigned long previousMillis = 0;
unsigned long currentDuration = shortFlash;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, HIGH);  
}
void loop() 
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= currentDuration) 
  {
    previousMillis = currentMillis;
    currentStep++;

    // S: 短闪 3 次
    if (currentStep == 1) {
      digitalWrite(ledPin, LOW);
      currentDuration = innerGap;
    }

    if (currentStep == 2) {
      digitalWrite(ledPin, HIGH);
      currentDuration = shortFlash;
    }

    if (currentStep == 3) {
      digitalWrite(ledPin, LOW);
      currentDuration = innerGap;
    }

    if (currentStep == 4) {
      digitalWrite(ledPin, HIGH);
      currentDuration = shortFlash;
    }

    if (currentStep == 5) {
      digitalWrite(ledPin, LOW);
      currentDuration = letterGap;
    }

    // O: 长闪 3 次
    if (currentStep == 6) {
      digitalWrite(ledPin, HIGH);
      currentDuration = longFlash;
    }

    if (currentStep == 7) {
      digitalWrite(ledPin, LOW);
      currentDuration = innerGap;
    }

    if (currentStep == 8) {
      digitalWrite(ledPin, HIGH);
      currentDuration = longFlash;
    }

    if (currentStep == 9) {
      digitalWrite(ledPin, LOW);
      currentDuration = innerGap;
    }

    if (currentStep == 10) {
      digitalWrite(ledPin, HIGH);
      currentDuration = longFlash;
    }

    if (currentStep == 11) {
      digitalWrite(ledPin, LOW);
      currentDuration = letterGap;
    }

    // S: 短闪 3 次
    if (currentStep == 12) {
      digitalWrite(ledPin, HIGH);
      currentDuration = shortFlash;
    }

    if (currentStep == 13) {
      digitalWrite(ledPin, LOW);
      currentDuration = innerGap;
    }

    if (currentStep == 14) {
      digitalWrite(ledPin, HIGH);
      currentDuration = shortFlash;
    }

    if (currentStep == 15) {
      digitalWrite(ledPin, LOW);
      currentDuration = innerGap;
    }

    if (currentStep == 16) {
      digitalWrite(ledPin, HIGH);
      currentDuration = shortFlash;
    }

    if (currentStep == 17) {
      digitalWrite(ledPin, LOW);
      currentDuration = sosGap;
    }

    // 一轮 SOS 结束，重新开始
    if (currentStep >= 18) {
      currentStep = 0;
      digitalWrite(ledPin, HIGH);
      currentDuration = shortFlash;
    }
  }
}