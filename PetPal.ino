#include <IRremote.h>
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
  rtc.adjust(DateTime(2023, 11, 14, 08, 16, 0));
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

    switch (results.value) {
    case 0xFF30CF:
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
  if (irCode == 0xFFE01F)
    return '1';
  else if (irCode == 0xFF906F)
    return '2';
  return '\0';
}

void setMotorStartTime(int hours, int minutes) {
  while (true) {
    tmElements_t tm;
    if (RTC.read(tm)) {
      if (tm.Hour == hours && tm.Minute == minutes) {
        Serial.println("Motor triggered!");
        motor.write(90);
        delay(1000);
        motor.write(0);
        break;
      }
    }
    delay(1000);
  }
}
