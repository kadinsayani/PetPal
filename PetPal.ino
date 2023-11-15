#include <IRremote.hpp>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Servo.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

const int motorPin = 10;
Servo motor;

RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  motor.attach(motorPin);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  rtc.adjust(DateTime(2023, 11, 14, 8, 16, 0));
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

    switch (results.value) {
    case 0xFD708F:
      setMotorTime();
      break;
    default:
      break;
    }

    irrecv.resume();
  }
}

void setMotorTime() {
  Serial.println("Enter the desired time using the remote (HH:MM format):");
  String timeString = "";
  while (!irrecv.decode(&results)) {
    // Wait for IR input
  }

  while (results.value != OK_BUTTON_IR_CODE) {
    if (irrecv.decode(&results)) {
      if (results.value == NUMBER_BUTTON_IR_CODE) {
        char buttonChar = getButtonChar(results.value);
        timeString += buttonChar;
        Serial.print(buttonChar);
      }
      irrecv.resume();
    }
  }

  int hours = timeString.substring(0, 2).toInt();
  int minutes = timeString.substring(3, 5).toInt();

  Serial.print("Setting motor to run at ");
  Serial.print(hours);
  Serial.print(":");
  Serial.println(minutes);

  setMotorStartTime(hours, minutes);
}

char getButtonChar(unsigned long irCode) {
  switch (irCode) {
  case 0xFD08F7:
    return '1';
  case 0xFD8877:
    return '2';
  case 0xFD48B7:
    return '3';
  case 0xFD28D7:
    return '4';
  case 0xFDA857:
    return '5';
  case 0xFD6897:
    return '6';
  case 0xFD18E7:
    return '7';
  case 0xFD9867:
    return '8';
  case 0xFD58A7:
    return '9';
  default:
    return '\0';
  }
}

void setMotorStartTime(int hours, int minutes) {
  while (true) {
    DateTime now = rtc.now();
    if (now.hour() == hours && now.minute() == minutes) {
      Serial.println("Motor triggered!");
      motor.write(90);
      delay(1000);
      motor.write(0);
      break;
    }
    delay(1000);
  }
}
