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
    int comma = input.indexOf(',');
    if (comma < 0) {
      Serial.println("Error: invalid format");
    } else {
      int pos1 = input.substring(0, comma).toInt();
      int pos2 = input.substring(comma + 1).toInt();
      
      Serial.print("output 1 going to: ");
      Serial.println(pos1);
      Serial.print("output 2 going to: ");
      Serial.println(pos2);
      
      out1.goTo(pos1);
      out2.goTo(pos2);
      Serial.println("OK");
    }
      
  }
}
