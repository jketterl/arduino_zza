#include "output.h"

Output::Output(int homePin, int motorPin, int flapPin) {
  _homePin = homePin;
  _motorPin = motorPin;
  _flapPin = flapPin;
  
  _state = STATE_STOPPED;
  _currentPosition = 0;
}

void Output::begin() {
  pinMode(_flapPin, INPUT);
  pinMode(_homePin, INPUT);
  
  home();
}

void Output::home() {
  motorOn();
  delay(MOTOR_SPINUP_DELAY);
  while (digitalRead(_homePin)) ;
  delay(MOTOR_SHUTOFF_DELAY);
  motorOff();
  _currentPosition = 0;
}

void Output::motorOn() {
  // motor on
  pinMode(_motorPin, OUTPUT);
  digitalWrite(_motorPin, HIGH);
}

void Output::motorOff() {
  // motor off
  pinMode(_motorPin, INPUT);
  digitalWrite(_motorPin, LOW);
}

void Output::setTarget(int target) {
  _target = target % FLAPCOUNT;
}

bool Output::process() {
  if (_state == STATE_STOPPED) {
    if (_target == _currentPosition) {
      // our state has reached a stable position; we are finished.
      return true;
    } else {
      motorOn();
      _state = STATE_SPINUP;
      _timestamp = millis() + MOTOR_SPINUP_DELAY;
    }
  } else if (_state == STATE_SPINUP) {
    if (_timestamp <= millis()) {
      if (_target < _currentPosition) {
        _state = STATE_HOMING;
      } else {
        _state = STATE_STEPPING;
      }
    }
  } else if (_state == STATE_HOMING) {
    if (!digitalRead(_homePin)) {
      _currentPosition = 0;
      motorOff();
      _timestamp = millis() + MOTOR_SHUTOFF_DELAY;
      _state = STATE_SPINDOWN;
    }
  } else if (_state == STATE_STEPPING) {
    if (_target == _currentPosition) {
      motorOff();
      _state = STATE_SPINDOWN;
      _timestamp = millis() + MOTOR_SHUTOFF_DELAY;
    } else {
      int sense = digitalRead(_flapPin);
      if (!_lastSense && sense) _currentPosition++;
      _lastSense = sense;
    }
  } else if (_state == STATE_SPINDOWN) {
    if (_timestamp <= millis()) {
      _state = STATE_STOPPED;
    }
  }
  
  return false;
}
