
#include <Arduino.h>
#include "boot_sequence.h"
#include "mpu_debug.h"

bool systemReady = false;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n=== ContaVogas MPU Debug ===");

    // Initialize I2C and verify that the sensor is detected.
    if (!runBootSequence()) {
        Serial.println("ERROR: Boot sequence failed.");
        return;
    }

    // Wake up and configure the MPU6050.
    if (!initMPUDebug()) {
        Serial.println("ERROR: MPU initialization failed.");
        return;
    }

    systemReady = true;
    Serial.println("ContaVogas ready!");
}

void loop() {
    if (!systemReady) {
        delay(1000);
        return;
    }

    // Read and print sensor values continuously.
    updateMPUDebug();

    delay(1000);
}
