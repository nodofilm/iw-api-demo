#include <Arduino.h>
#include "iw_api.h"
    
iw_api_c inertia_wheels;

void data_print();

void setup() {
    Serial.begin(115200);
    Serial1.begin(460800);
    strcpy(inertia_wheels.control.name, "API Demo");
    strcpy(inertia_wheels.control.status, "System Okay");
    inertia_wheels.control.status_level = OKAY;
    strcpy(inertia_wheels.control.lens_status, "ok. ok. ok.");
    strcpy(inertia_wheels.control.knob1.name, "Setting 1");
    strcpy(inertia_wheels.control.knob2.name, "Setting 2");
    inertia_wheels.control.battery_left = 13.1;
    inertia_wheels.control.battery_right = 11.9;
    inertia_wheels.control.battery_warning_voltage = 12.0;
}

void loop() {
    static int count;
    if (Serial1.available()){
        inertia_wheels.injest(Serial1.read());
        if (inertia_wheels.new_packet()){
            data_print();
            count ++;
            if (count >= 10){
                count = 0;
                inertia_wheels.build_control_packet();
                for (unsigned int i=0; i < SIZE_OF_API_REPLY; i++) {
                    Serial1.write(inertia_wheels.control_packet[i]);
                    // Serial.write(inertia_wheels.control_packet[i]);
                    // Serial.print("_");
                }
                // Serial.println();
            }
        }
    }
}

void data_print() {
    Serial.println("INCOMING DATA");
    Serial.println(inertia_wheels.data.pan);
    Serial.println(inertia_wheels.data.tilt);
    Serial.println(inertia_wheels.data.focus);
    Serial.println(inertia_wheels.data.iris);
    Serial.println(inertia_wheels.data.zoom);
    Serial.println(inertia_wheels.data.rssi);
    Serial.println(inertia_wheels.data.snr);
    Serial.println();
}