// Entrypoint: servo exercise. Steps the servo 0 -> 90 -> 180 and back down, 90 degrees at a time, forever.
//   pio run -e servoTest -t upload -t monitor
#include <Arduino.h>
#include "Servo.h"

RobotServo servo;

constexpr int stepDeg = 90;
constexpr int maxDeg = 180;          // servoWrite clamps to 0..180
constexpr unsigned long holdMs = 1000; // time to reach and hold each position

void moveTo(int degrees) {
    servo.write(degrees);
    Serial.printf("servo -> %d deg\n", degrees);
    delay(holdMs);
}

void setup() {
    Serial.begin(115200);
    servo.begin(); // goes to Constants::ServoConfig::startDeg
    delay(holdMs);
}

void loop() {
    servo.write(45);
    delay(5000);
    servo.write(105);
    delay(5000);
    delay(100000);
}
