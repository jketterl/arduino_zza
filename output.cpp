#include "output.h"

Output::Output(int homePin, int motorPin, int flapPin) {
  _homePin = homePin;
  _motorPin = motorPin;
  _flapPin = flapPin;
}

void Output::begin() {
  pinMode(_flapPin, INPUT);
  pinMode(_homePin, INPUT);
  
  home();
}

void Output::home() {
  motorOn();
  while (digitalRead(_homePin)) ;
  motorOff();
  _currentPosition = 0;
}

void Output::motorOn() {
  // motor on
  pinMode(_motorPin, OUTPUT);
  digitalWrite(_motorPin, HIGH);
}

void Output::motorOff() {
  delay(MOTOR_SHUTOFF_DELAY);
  
  // motor off
  pinMode(_motorPin, INPUT);
  digitalWrite(_motorPin, LOW);
}

void Output::goTo(int position) {
  position %= 40;
  int steps = (40 + position - _currentPosition) % 40;
  if (_currentPosition + steps > 39) {
    home();
    steps = position;
  }
  step(steps);
  _currentPosition = position;
}

void Output::step(int count) {
  count %= 40;
  if (count == 0) return;
  motorOn();
  // motor spinup time
  delay(MOTOR_SPINUP_DELAY);

  //wait for amount of steps to pass by
  while (count-- > 0) {
    while (!digitalRead(_flapPin)) ;
    while (digitalRead(_flapPin)) ;
  }
  
  motorOff();
}
