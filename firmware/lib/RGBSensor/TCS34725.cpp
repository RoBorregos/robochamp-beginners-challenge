#include "TCS34725.h"
#include "constants.h"
#include <Arduino.h>
#include <Wire.h>

namespace {
    constexpr uint8_t addr = 0x29;
    constexpr uint8_t cmd = 0x80 | 0x20;      // command bit + auto-increment
    constexpr uint8_t regEnable = 0x00, regAtime = 0x01, regControl = 0x0F, regId = 0x12, regCdata = 0x14;
    constexpr uint8_t pon = 0x01, aen = 0x02;
    constexpr uint8_t atime = 0xEB;           // (256 - 0xEB) * 2.4 ms = ~50 ms integration
    constexpr uint8_t gain4x = 0x01;

    void write8(uint8_t reg, uint8_t v) {
        Wire.beginTransmission(addr);
        Wire.write(cmd | reg);
        Wire.write(v);
        Wire.endTransmission();
    }

    // Reads n bytes starting at reg. False on any I2C error.
    bool readN(uint8_t reg, uint8_t* buf, uint8_t n) {
        Wire.beginTransmission(addr);
        Wire.write(cmd | reg);
        if (Wire.endTransmission(false) != 0) return false;
        if (Wire.requestFrom(addr, n) != n) return false;
        for (uint8_t i = 0; i < n; i++) buf[i] = Wire.read();
        return true;
    }
}

TCS34725::TCS34725(int sda, int scl) : _sda(sda), _scl(scl) {}

bool TCS34725::begin() {
    Wire.begin(_sda, _scl);
    uint8_t id = 0;
    if (!readN(regId, &id, 1) || (id != 0x44 && id != 0x4D)) { // 0x44 = TCS34721/5, 0x4D = TCS34723/7
        Serial.printf("TCS34725 not found (id 0x%02X), check SDA=%d SCL=%d\n", id, _sda, _scl);
        return false;
    }
    write8(regAtime, atime);
    write8(regControl, gain4x);
    write8(regEnable, pon);
    delay(3); // datasheet: 2.4 ms warm-up after power on
    write8(regEnable, pon | aen); // continuous conversions; reads return the latest result
    return true;
}

RGB TCS34725::readRGB() {
    uint8_t b[8]; // C, R, G, B as 16-bit little endian
    if (!readN(regCdata, b, sizeof(b))) return {0, 0, 0};
    return {(float)(b[2] | b[3] << 8), (float)(b[4] | b[5] << 8), (float)(b[6] | b[7] << 8)};
}

// Brightness-independent features: each channel as a share of the total. False if the sensor read nothing.
bool TCS34725::readFeatures(float f[3]) {
    const RGB c = readRGB();
    const float total = c.r + c.g + c.b;
    if (total <= 0.0f) return false;
    f[0] = c.r / total;
    f[1] = c.g / total;
    f[2] = c.b / total;
    return true;
}

// shortcut: the 0.10 / 0.08 margins are guessed, not measured; tune on the real track.
Constants::ColorsOnTrack TCS34725::readColor() {
    using C = Constants::ColorsOnTrack;
    float f[3];
    if (!readFeatures(f)) return C::Gray;
    const float r = f[0], g = f[1], b = f[2];

    if (fabsf(r - g) < 0.10f && fabsf(r - b) < 0.10f && fabsf(g - b) < 0.10f) {
        return C::Gray;   // Los tres canales son aproximadamente iguales, por lo que es gris (o negro o blanco)
    } else if (fabsf(r - g) < 0.08f && r > b && g > b) {
        return C::Yellow; // Amarillo
    } else if (r > g && r > b) {
        return C::Red;    // Rojo
    } else if (g > r && g > b) {
        return C::Green;
    } else if (b > r && b > g) {
        return C::Blue;
    } else {
        return C::Gray;
    }
}
