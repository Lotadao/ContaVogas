
#include <Arduino.h>
#include <Wire.h>
#include "boot_sequence.h"

namespace {
    constexpr uint8_t MPU_ADDR_1 = 0x68;
    constexpr uint8_t MPU_ADDR_2 = 0x69;
    constexpr uint8_t WHO_AM_I_REG = 0x75;

    bool checkMPU6050() {
        const uint8_t addresses[] = {
            MPU_ADDR_1, MPU_ADDR_2
        };

        for (uint8_t address : addresses) {
            Wire.beginTransmission(address);
            Wire.write(WHO_AM_I_REG);

            if (Wire.endTransmission(false) != 0) {
                continue;
            }

            if (Wire.requestFrom(address, (uint8_t)1) != 1) {
                continue;
            }

            uint8_t identity = Wire.read();

            if (identity == 0x68) {
                Serial.printf(
                    "MPU6050 detected at 0x%02X\n",
                    address
                );
                return true;
            }
        }

        Serial.println("ERROR: MPU6050 not detected.");
        return false;
    }
}

bool runBootSequence() {
    Serial.println("\n=== ContaVogas Boot ===");

    Serial.println("[1/2] Initializing I2C...");
    if (!Wire.begin(21, 22)) {
        Serial.println("ERROR: I2C initialization failed.");
        return false;
    }

    Wire.setClock(100000);
    delay(50);

    Serial.println("[2/2] Checking MPU6050...");
    if (!checkMPU6050()) {
        Serial.println("BOOT FAILED.");
        return false;
    }

    Serial.println("BOOT SUCCESSFUL.");
    return true;
}
