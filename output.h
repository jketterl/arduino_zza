#ifndef output_h
#define output_h

#include "Arduino.h"

#define MOTOR_SHUTOFF_DELAY 0
#define MOTOR_SPINUP_DELAY 20

class Output {
  public:
    Output(int homePin, int motorPin, int flapPin);
    void begin();
    void home();
    void goTo(int position);
  private:
    int _homePin;
    int _motorPin;
    int _flapPin;
    int _currentPosition;
    
    void motorOn();
    void motorOff();
    void step(int count);
};

#endif
