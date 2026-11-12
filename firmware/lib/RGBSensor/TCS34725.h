#pragma once

#include "constants.h"

// Raw channel counts (higher = more of that color). Not calibrated units.
struct RGB {
    float r, g, b;
};

// TCS34725 over I2C (address 0x29), plain Wire: no extra library.
class TCS34725 {
public:
    TCS34725(int sda, int scl);
    bool begin();                                     // false if the sensor does not answer on I2C
    RGB readRGB();                                    // all zero if the read failed
    Constants::ColorsOnTrack readColor();             // if-chain on channel shares

private:
    int _sda, _scl;
    bool readFeatures(float f[3]);
};
