#include <Arduino.h>
#include "iw_api.h"
    
iw_api_c inertia_wheels;

void setup() {
    Serial.begin(115200);
    Serial1.begin(460800);
}

void loop() {
    if (Serial1.available()){
        inertia_wheels.injest(Serial1.read());
        if (inertia_wheels.new_packet()){
            Serial.println("New Packet");
        }
    }
}