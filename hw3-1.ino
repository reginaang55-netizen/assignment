const int buttonPin = 2;
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

bool lastReading = HIGH;
bool buttonState = HIGH;
bool longPressDone = false;

unsigned long debounceTime = 0;
unsigned long pressTime = 0;
unsigned long blinkTime = 0;
unsigned long fadeTime = 0;

int colorIndex = 0;
int mode = 0;  // 0恒亮、1闪烁、2呼吸
bool lightOn = true;

int brightness = 0;
int fadeAmount = 5;

const int colors[7][3] = {
  {255, 0, 0},
  {0, 255, 0},
  {0, 0, 255},
  {255, 255, 0},
  {255, 0, 255},
  {0, 255, 255},
  {255, 255, 255}
};

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  checkButton();
  updateLight();
}

void checkButton() {
  unsigned long now = millis();
  bool reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    debounceTime = now;
  }
  lastReading = reading;

  if (now - debounceTime >= 30) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        pressTime = now;
        longPressDone = false;
      } else if (!longPressDone) {
        colorIndex = (colorIndex + 1) % 7;
        Serial.println("Change color");
      }
    }
  }
  if (buttonState == LOW &&
      !longPressDone &&
      now - pressTime >= 1000) {
    longPressDone = true;

    mode = (mode + 1) % 3;

    lightOn = true;
    blinkTime = now;
    fadeTime = now;
    brightness = 0;
    fadeAmount = 5;

    Serial.print("Mode: ");
    Serial.println(mode);
  }
}

void updateLight() {
  unsigned long now = millis();
  int level = 255;

  if (mode == 0) {
    level = 255;
  } else if (mode == 1) {
    if (now - blinkTime >= 500) {
      blinkTime = now;
      lightOn = !lightOn;
    }

    level = lightOn ? 255 : 0;
  } else {
    if (now - fadeTime >= 20) {
      fadeTime = now;
      brightness += fadeAmount;

      if (brightness >= 255) {
        brightness = 255;
        fadeAmount = -5;
      } else if (brightness <= 0) {
        brightness = 0;
        fadeAmount = 5;
      }
    }

    level = brightness;
  }

  // 使用 long，避免亮度相乘时超出整数范围
  int red = (long)colors[colorIndex][0] * level / 255;
  int green = (long)colors[colorIndex][1] * level / 255;
  int blue = (long)colors[colorIndex][2] * level / 255;

  setColor(red, green, blue);
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, 255 - red);
  analogWrite(greenPin, 255 - green);
  analogWrite(bluePin, 255 - blue);
}