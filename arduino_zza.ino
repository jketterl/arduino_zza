#define HOME_SENSE 2
#define MOTOR_PIN 3
#define MOTOR_SENSE 4
#define LED 13
#define MOTOR_SHUTOFF_DELAY 0

#include "output.h"

Output out(5, 6, 7);

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTOR_SENSE, INPUT);
  pinMode(LED, OUTPUT);
  
  home();
  
  out.begin();
  
  Serial.begin(9600);
}

int currentPosition;

void motorOn() {
  // led on
  digitalWrite(LED, HIGH);
  
  // motor on
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, HIGH);
}

void motorOff() {
  delay(MOTOR_SHUTOFF_DELAY);
  
  // led off
  digitalWrite(LED, LOW); 
  
  // motor off
  pinMode(MOTOR_PIN, INPUT);
  digitalWrite(MOTOR_PIN, LOW);
}

void step(int count) {
  count %= 40;
  if (count == 0) return;
  motorOn();
  // motor spinup time
  delay(20);

  //wait for amount of steps to pass by
  while (count-- > 0) {
    while (!digitalRead(MOTOR_SENSE)) ;
    while (digitalRead(MOTOR_SENSE)) ;
  }
  
  motorOff();
}

void home() {
  motorOn();
  while (digitalRead(HOME_SENSE)) ;
  motorOff();
  currentPosition = 0;
}

void goTo(int position) {
  position %= 40;
  Serial.print("going to ");
  Serial.print(position);
  int steps = (40 + position - currentPosition) % 40;
  Serial.print(" steps ");
  Serial.print(steps);
  if (currentPosition + steps > 39) {
    Serial.print(" homing; remaining steps: ");
    home();
    steps = position;
    Serial.print(steps);
  }
  step(steps);
  currentPosition = position;
}

void loop() {
  String input = Serial.readStringUntil('\n');
  if (input == "") {
    // noop
  } else if (input == "home") {
    Serial.print("homing...");
    home();
    Serial.println(" OK");
  } else {
    int pos = input.toInt();
    Serial.print("going to ");
    Serial.print(pos);
    Serial.print("...");
    goTo(pos);
    out.goTo(pos);
    Serial.println(" OK");
  }
}
