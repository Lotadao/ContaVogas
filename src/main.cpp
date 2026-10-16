
#include <Arduino.h>

// Function implemented in boot_sequence.cpp
bool runBootSequence();

void setup() {
    Serial.begin(115200);
    delay(1000);

    bool bootOK = runBootSequence();

    if (bootOK) {
        Serial.println("System ready for rowing.");
    } else {
        Serial.println("System error. Check diagnostics.");
    }
}

void loop() {
    // Rowing coach functionality will be added later.
}
