
#include <Arduino.h>
#include <Wire.h>
#include "mpu_debug.h"

namespace {
    constexpr uint8_t MPU_ADDR = 0x68;

    constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
    constexpr uint8_t REG_TEMP_OUT_H   = 0x41;
    constexpr uint8_t REG_GYRO_XOUT_H  = 0x43;

    bool readRegisterBlock(
        uint8_t startRegister,
        uint8_t* buffer,
        size_t length
    ) {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(startRegister);

        if (Wire.endTransmission(false) != 0) {
            return false;
        }

        size_t received = Wire.requestFrom(
            (uint8_t)MPU_ADDR,
            (uint8_t)length
        );

        if (received != length) {
            return false;
        }

        for (size_t i = 0; i < length; i++) {
            buffer[i] = Wire.read();
        }

        return true;
    }

    int16_t combineBytes(uint8_t high, uint8_t low) {
        return (int16_t)(
            ((uint16_t)high << 8) | low
        );
    }
}

void printMPUValues() {
    uint8_t accelData[6];
    uint8_t tempData[2];
    uint8_t gyroData[6];

    if (!readRegisterBlock(
            REG_ACCEL_XOUT_H, accelData, 6)) {
        Serial.println("ERROR: Cannot read accelerometer!");
        return;
    }

    if (!readRegisterBlock(
            REG_TEMP_OUT_H, tempData, 2)) {
        Serial.println("ERROR: Cannot read temperature!");
        return;
    }

    if (!readRegisterBlock(
            REG_GYRO_XOUT_H, gyroData, 6)) {
        Serial.println("ERROR: Cannot read gyroscope!");
        return;
    }

    int16_t axRaw = combineBytes(accelData[0], accelData[1]);
    int16_t ayRaw = combineBytes(accelData[2], accelData[3]);
    int16_t azRaw = combineBytes(accelData[4], accelData[5]);

    int16_t tempRaw = combineBytes(tempData[0], tempData[1]);

    int16_t gxRaw = combineBytes(gyroData[0], gyroData[1]);
    int16_t gyRaw = combineBytes(gyroData[2], gyroData[3]);
    int16_t gzRaw = combineBytes(gyroData[4], gyroData[5]);

    // Default MPU6050 ranges after reset:
    // Accelerometer: +/- 2 g (16384 LSB/g)
    // Gyroscope:     +/- 250 degrees/second (131 LSB/(deg/s))
    float ax = axRaw / 16384.0f;
    float ay = ayRaw / 16384.0f;
    float az = azRaw / 16384.0f;

    float gx = gxRaw / 131.0f;
    float gy = gyRaw / 131.0f;
    float gz = gzRaw / 131.0f;

    // MPU6050 temperature formula from its datasheet.
    float temperatureC = (tempRaw / 340.0f) + 36.53f;

    Serial.println("----- MPU6050 -----");

    Serial.printf(
        "Accel (g):     X=%7.3f Y=%7.3f Z=%7.3f\n",
        ax, ay, az
    );

    Serial.printf(
        "Gyro (deg/s):  X=%7.2f Y=%7.2f Z=%7.2f\n",
        gx, gy, gz
    );

    Serial.printf(
        "Temperature:   %.2f C\n",
        temperatureC
    );

    Serial.printf(
        "Accel raw:     X=%d Y=%d Z=%d\n",
        axRaw, ayRaw, azRaw
    );

    Serial.printf(
        "Gyro raw:      X=%d Y=%d Z=%d\n",
        gxRaw, gyRaw, gzRaw
    );

    Serial.println();
}

void runMPUDebug() {
    Serial.println("Starting MPU6050 debug mode...");

    while (true) {
        printMPUValues();
        delay(250); // Four readings per second
    }
}
