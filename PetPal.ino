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
  if (now.unixtime() >= targetTime.unixtime()) {
    Serial.println("Motor triggered!");
    motor.write(90);
    delay(5000);
    motor.write(0);
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
  if (IrReceiver.decode()) {
    char buttonChar = getButtonChar(IrReceiver.decodedIRData.decodedRawData);
    Serial.println(buttonChar);
    IrReceiver.resume();
  }
}

char getButtonChar(uint32_t irCode) {
  switch (irCode) {
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
