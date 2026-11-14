#include "Servo.h"
#include "constants.h"
#include <Arduino.h>
#include <esp_arduino_version.h>

#if ESP_ARDUINO_VERSION_MAJOR < 3
#error "This project needs ESP32 Arduino core 3.x. Run 'pio pkg install' in firmware/ (or setup.bat)."
#endif

// Plain core LEDC PWM, no servo library: ledcAttach() always takes a free channel and gives 50 Hz its own timer,
// so the servo can't end up sharing a channel with the motors' / LED's analogWrite (which happened with ESP32Servo).
namespace {
    constexpr int bits = 14;                         // duty resolution
    constexpr int periodUs = 1000000 / Constants::ServoConfig::hz; // 20000 us at 50 Hz
}

void servoBegin() {
    ledcAttach(Constants::ServoConfig::pin, Constants::ServoConfig::hz, bits);
    servoWrite(Constants::ServoConfig::startDeg);
}

void servoWrite(int degrees) {
    using namespace Constants::ServoConfig;
    const int pulseUs = map(constrain(degrees, 0, 180), 0, 180, minUs, maxUs);
    ledcWrite(pin, (uint32_t)pulseUs * (1 << bits) / periodUs);
}
