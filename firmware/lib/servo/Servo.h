#pragma once

// Hobby servo on Constants::ServoConfig (pin, 50 Hz, pulse range).
void servoBegin();               // attach the servo and move it to startDeg
void servoWrite(int degrees);    // move to an angle, clamped to 0..180
