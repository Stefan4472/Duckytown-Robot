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

class LightsInterface
{
  private:
    // Pin assignments.
    int leftPin, rightPin;

    int blinkerState = BlinkerState::NOT_BLINKING;
    int brakeOn = false;
    unsigned long lastBlinkMs;

  public:
    // Pin for left light, and for right light.
    void init(int leftPin, int rightPin);
    // Updates light states.
    void update();
    // Turn signals.
    void startLeftBlinker();
    void stopLeftBlinker();
    void startRightBlinker();
    void stopRightBlinker();
    // Brake.
    void startBrakeLight();
    void stopBrakeLight();
};

#endif