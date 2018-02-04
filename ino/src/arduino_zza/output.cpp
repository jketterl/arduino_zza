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
      _lastSense = 0;
      motorOn();
      _state = STATE_SPINUP;
      _timestamp = millis() + MOTOR_SPINUP_DELAY;
    }
  } else if (_state == STATE_SPINUP) {
    int nextState = _target < _currentPosition ? STATE_HOMING : STATE_STEPPING;
    // during spinup the flap data line might go low if the motor has reveresed a little after being stopped.
    // if this happens, start the stepping process immediately.
    // if not, we wait for the timeout and store a correction value which is necessary when stepping for a single step.
    if (_timestamp <= millis()) {
      // no flap event during spinup -> timeout
      _state = nextState;
      _spinupCorrection = MOTOR_SPINUP_CORRECTION;
    } else {
      // detect flaps
      int sense = digitalRead(_flapPin);
      if (_lastSense && !sense) {
        // flap detected. go to next state
        _state = nextState;
        _spinupCorrection = 0;
      }
      _lastSense = sense;
    }
  } else if (_state == STATE_HOMING) {
    if (!digitalRead(_homePin)) {
      _currentPosition = 0;
      motorOff();
      _timestamp = millis() + MOTOR_SHUTOFF_DELAY;
      _state = STATE_SPINDOWN;
    }
  } else if (_state == STATE_STEPPING) {
    if (_target == _currentPosition + 1) {
      _state = STATE_LAST_FLAP;
      _timestamp = millis() + MOTOR_FLAP_DELAY - _spinupCorrection;
    } else {
      int sense = digitalRead(_flapPin);
      if (_lastSense && !sense) {
        _currentPosition++;
        _spinupCorrection = 0;
      }
      _lastSense = sense;
    }
  } else if (_state == STATE_LAST_FLAP) {
    if (_timestamp <= millis()) {
      motorOff();
      _currentPosition = _target;
      _state = STATE_SPINDOWN;
      _timestamp = millis() + MOTOR_SHUTOFF_DELAY;
    } else {
      // emergency brake if the flap line goes low before we expect it to
      int sense = digitalRead(_flapPin);
      if (_lastSense && !sense) {
        motorOff();
        _currentPosition = _target;
        _state = STATE_SPINDOWN;
        _timestamp = millis() + MOTOR_SHUTOFF_DELAY;
      }
      _lastSense = sense;
    }
  } else if (_state == STATE_SPINDOWN) {
    if (_timestamp <= millis()) {
      _state = STATE_STOPPED;
    }
  }
  
  return false;
}
