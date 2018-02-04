#ifndef output_h
#define output_h

#include "Arduino.h"

#define MOTOR_SHUTOFF_DELAY 0
#define MOTOR_FLAP_DELAY 50
#define MOTOR_SPINUP_DELAY 40
#define MOTOR_SPINUP_CORRECTION MOTOR_SPINUP_DELAY - 10
#define FLAPCOUNT 40

#define STATE_STOPPED   0
#define STATE_SPINUP    1
#define STATE_STEPPING  2
#define STATE_SPINDOWN  3
#define STATE_HOMING    4
#define STATE_LAST_FLAP 5

class Output {
  public:
    Output(int homePin, int motorPin, int flapPin);
    void begin();
    void home();
    void setTarget(int position);
    bool process();
  private:
    int _homePin;
    int _motorPin;
    int _flapPin;
    int _currentPosition;
    int _target;
    int _state;
    long _timestamp;
    int _lastSense = 1;
    int _spinupCorrection;
    
    void motorOn();
    void motorOff();
};

#endif
