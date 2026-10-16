

#include <Arduino.h>

// Run startup diagnostics
bool runBootSequence() {
    bool systemOK = true;

    Serial.println();
    Serial.println("==============================");
    Serial.println("      ROWING COACH");
    Serial.println("      SYSTEM BOOT");
    Serial.println("==============================");

    // Test 1: Serial communication
    if (Serial) {
        Serial.println("[ OK ] Serial communication");
    } else {
        Serial.println("[FAIL] Serial communication");
        systemOK = false;
    }

    // Test 2: ESP32 chip information
    if (ESP.getChipModel() != nullptr) {
        Serial.print("[ OK ] ESP32 detected: ");
        Serial.println(ESP.getChipModel());
    } else {
        Serial.println("[FAIL] ESP32 chip information");
        systemOK = false;
    }

    // Test 3: Free memory
    uint32_t freeHeap = ESP.getFreeHeap();

    Serial.print("[INFO] Free heap: ");
    Serial.print(freeHeap);
    Serial.println(" bytes");

    if (freeHeap > 10000) {
        Serial.println("[ OK ] Memory check");
    } else {
        Serial.println("[WARN] Low free memory");
    }

    // Boot summary
    Serial.println("------------------------------");

    if (systemOK) {
        Serial.println("BOOT RESULT: BASIC CHECKS PASSED");
    } else {
        Serial.println("BOOT RESULT: CHECK FAILED");
    }

    Serial.println("==============================");

    return systemOK;
}
