#define HOME_SENSE 2
#define MOTOR_PIN 3
#define MOTOR_SENSE 4
#define LED 13
#define MOTOR_SHUTOFF_DELAY 0

#include "output.h"

Output out1(2, 3, 4);
Output out2(5, 6, 7);

void setup() {
  out1.begin();
  out2.begin();
  
  Serial.begin(9600);
}

void loop() {
  String input = Serial.readStringUntil('\n');
  if (input == "") {
    // noop
  } else if (input == "home") {
    Serial.print("homing...");
    out1.home();
    out2.home();
    Serial.println(" OK");
  } else {
    int pos = input.toInt();
    Serial.print("going to ");
    Serial.print(pos);
    Serial.print("...");
    out1.goTo(pos);
    out2.goTo(pos);
    Serial.println(" OK");
  }
}
