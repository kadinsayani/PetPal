#include <IRremote.h>
#include <Keypad.h>
#include <RTClib.h>
#include <Servo.h>

const uint32_t OK_BUTTON_IR_CODE = 0xF20DFF00;
const int IR_RECEIVE_PIN = 11;

const int motorPin = 9;
Servo motor;

RTC_DS1307 rtc;
DateTime now;
DateTime targetTime;

int userInputCount = 0;
char userInput[4];
bool motorTriggered = false;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  motor.attach(motorPin);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  now = rtc.now();
  targetTime = now + TimeSpan(0, 1, 0, 0);
  Serial.println(now.unixtime());
  Serial.println(targetTime.unixtime());
}

void loop() {
  now = rtc.now();
  Serial.println(String(now.unixtime()));
  delay(5000);
  Serial.println(String(targetTime.unixtime()));

  if (now.unixtime() >= targetTime.unixtime()) {
    motorTriggered = true;
  }

  while (motorTriggered) {
    Serial.println("Motor triggered!");
    motor.write(180 - 45);
    delay(5000);
    motor.write(180 - 100);
    delay(5000);
    motorTriggered = false;
  }

  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    if (IrReceiver.decodedIRData.decodedRawData == OK_BUTTON_IR_CODE) {
      Serial.println("OK button pressed!");
      setMotorTime();
    }
    IrReceiver.resume();
  }
}

void setMotorTime() {
  Serial.println("Setting motor time...");
  userInputCount = 0;
  memset(userInput, 0, sizeof(userInput));

  while (userInputCount < 4) {
    if (IrReceiver.decode()) {
      char buttonChar = getButtonChar(IrReceiver.decodedIRData.decodedRawData);
      if (buttonChar >= '0' && buttonChar <= '9') {
        Serial.print("Button pressed: ");
        Serial.println(buttonChar);

        userInput[userInputCount] = buttonChar;
        userInputCount++;
      }

      IrReceiver.resume();
    }
  }

  int hours = (userInput[0] - '0') * 10 + (userInput[1] - '0');
  int minutes = (userInput[2] - '0') * 10 + (userInput[3] - '0');

  targetTime = DateTime(now.year(), now.month(), now.day(), hours, minutes, 0);
  Serial.println("Target time set to: " + String(targetTime.unixtime()));
}

char getButtonChar(uint32_t irCode) {
  switch (irCode) {
  case 0xE916FF00:
    return '0';
  case 0xF30CFF00:
    return '1';
  case 0xE718FF00:
    return '2';
  case 0xA15EFF00:
    return '3';
  case 0xF708FF00:
    return '4';
  case 0xE31CFF00:
    return '5';
  case 0xA55AFF00:
    return '6';
  case 0xBD42FF00:
    return '7';
  case 0xAD52FF00:
    return '8';
  case 0xB54AFF00:
    return '9';
  default:
    return '\0';
  }
}
