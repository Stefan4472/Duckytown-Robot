#ifndef LIGHTS_H
#define LIGHTS_H

// Time between toggling the blinker (when turning)
#define BLINK_PERIOD_MS 250

enum BlinkerState
{
  BLINKING_LEFT,
  BLINKING_RIGHT,
  NOT_BLINKING
};

/*
Brake light and turn signal control. 
Turn signals toggle state every 'BLINK_PERIOD_MS' while flashing.
The brake light takes priority over turn signals, but will not
destroy turn signal state (i.e. the turn signals keep updating 
their flash logic and will continue blinking once the brake 
light is off). 
Only one turn signal can be blinking at a time (left or right).
Initialize the class with the digital pin for the left and right
lights.
*/
class LightsInterface
{
  private:
    // Pin assignments.
    int leftPin, rightPin;
    // Whether brake light is currently on.
    int brakeOn = false;
    // Current state of turn signal.
    BlinkerState blinkerState = BlinkerState::NOT_BLINKING;
    // Timestamp of the last LED toggle for turn signalling.
    unsigned long lastBlinkMs;

  public:
    // Pins for left and right lights.
    void init(int leftPin, int rightPin);
    // Updates light states.
    void update();
    // Start/stop turn signals.
    void startLeftBlinker();
    void stopLeftBlinker();
    void startRightBlinker();
    void stopRightBlinker();
    // Start/stop brake light.
    void startBrakeLight();
    void stopBrakeLight();
};

#endif