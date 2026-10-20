
#include <Arduino.h>
#include <Wire.h>
#include "mpu_debug.h"

namespace {
    uint8_t mpuAddress = 0x68;

    bool writeRegister(uint8_t reg, uint8_t value) {
        Wire.beginTransmission(mpuAddress);
        Wire.write(reg);
        Wire.write(value);
        return Wire.endTransmission() == 0;
    }

    bool readRegisters(uint8_t reg, uint8_t *data, size_t length) {
        Wire.beginTransmission(mpuAddress);
        Wire.write(reg);

        if (Wire.endTransmission(false) != 0) {
            return false;
        }

        size_t count = Wire.requestFrom(
            mpuAddress, (uint8_t)length
        );

        if (count != length) {
            while (Wire.available()) {
                Wire.read();
            }
            return false;
        }

        for (size_t i = 0; i < length; i++) {
            data[i] = Wire.read();
        }

        return true;
    }

    int16_t toInt16(uint8_t high, uint8_t low) {
        return (int16_t)(((uint16_t)high << 8) | low);
    }
}

bool initMPUDebug() {
    Serial.println("\nInitializing MPU6050...");

    // Find the actual I2C address.
    const uint8_t addresses[] = {0x68, 0x69};
    bool found = false;

    for (uint8_t address : addresses) {
        Wire.beginTransmission(address);

        if (Wire.endTransmission() == 0) {
            mpuAddress = address;
            found = true;
            break;
        }
    }

    if (!found) {
        Serial.println("ERROR: MPU6050 not responding.");
        return false;
    }

    Serial.printf("MPU6050 address: 0x%02X\n", mpuAddress);

    // Verify the sensor identity.
    uint8_t identity = 0;

    if (!readRegisters(0x75, &identity, 1)) {
        Serial.println("ERROR: Cannot read WHO_AM_I.");
        return false;
    }

    Serial.printf("WHO_AM_I: 0x%02X\n", identity);

    if (identity != 0x68) {
        Serial.println("ERROR: Unexpected sensor identity.");
        return false;
    }

    // IMPORTANT: Clear the sleep bit.
    // PWR_MGMT_1 = 0 wakes the MPU6050 using its
    // default internal clock.
    if (!writeRegister(0x6B, 0x00)) {
        Serial.println("ERROR: Cannot wake MPU6050.");
        return false;
    }

    delay(100);

    // Set accelerometer range to +/- 2 g.
    if (!writeRegister(0x1C, 0x00)) {
        Serial.println("ERROR: Cannot configure accelerometer.");
        return false;
    }

    // Set gyroscope range to +/- 250 degrees/second.
    if (!writeRegister(0x1B, 0x00)) {
        Serial.println("ERROR: Cannot configure gyroscope.");
        return false;
    }

    Serial.println("MPU6050 initialized successfully.");
    Serial.println("Move the sensor to see changing values.\n");

    return true;
}

void updateMPUDebug() {
    // Registers 0x3B through 0x48 contain acceleration,
    // temperature, and gyroscope data (14 bytes total).
    uint8_t data[14];

    if (!readRegisters(0x3B, data, sizeof(data))) {
        Serial.println("ERROR: Failed to read sensor data.");
        return;
    }

    int16_t axRaw = toInt16(data[0], data[1]);
    int16_t ayRaw = toInt16(data[2], data[3]);
    int16_t azRaw = toInt16(data[4], data[5]);

    int16_t tempRaw = toInt16(data[6], data[7]);

    int16_t gxRaw = toInt16(data[8], data[9]);
    int16_t gyRaw = toInt16(data[10], data[11]);
    int16_t gzRaw = toInt16(data[12], data[13]);

    // Convert raw readings to physical units.
    float ax = axRaw / 16384.0f;
    float ay = ayRaw / 16384.0f;
    float az = azRaw / 16384.0f;

    float gx = gxRaw / 131.0f;
    float gy = gyRaw / 131.0f;
    float gz = gzRaw / 131.0f;

    float temperature = tempRaw / 340.0f + 36.53f;

    Serial.printf(
        "Accel (g): X=%7.3f Y=%7.3f Z=%7.3f | ",
        ax, ay, az
    );

    Serial.printf(
        "Gyro (deg/s): X=%7.2f Y=%7.2f Z=%7.2f | ",
        gx, gy, gz
    );

    Serial.printf("Temp: %.2f C\n", temperature);

    Serial.printf(
        "RAW accel: X=%6d Y=%6d Z=%6d | "
        "RAW gyro: X=%6d Y=%6d Z=%6d\n\n",
        axRaw, ayRaw, azRaw,
        gxRaw, gyRaw, gzRaw
    );
}
