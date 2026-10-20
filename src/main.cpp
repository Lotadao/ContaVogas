
#include <Arduino.h>
#include "boot_sequence.h"
#include "mpu_debug.h"

bool systemReady = false;

void setup() {
    Serial.begin(115200);
    delay(1000);

    systemReady = runBootSequence();

    if (systemReady) {
        Serial.println("ContaVogas ready!");
        runMPUDebug();
    } else {
        Serial.println("Startup failed. Check hardware.");
    }
}

void loop() {
    // Debug mode runs inside runMPUDebug().
}
