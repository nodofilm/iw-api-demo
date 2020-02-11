#include <Arduino.h>
#include "iw_api.h"
    
iw_api_c inertia_wheels;

void setup() {
    Serial.begin(115200);
    Serial1.begin(460800);
    strcpy(inertia_wheels.control.name, "API Demo");
    strcpy(inertia_wheels.control.status, "System Okay");
    inertia_wheels.control.status_level = OKAY;
    strcpy(inertia_wheels.control.focus_name, "Focus");
    strcpy(inertia_wheels.control.iris_name, "Iris");
    strcpy(inertia_wheels.control.zoom_name, "Zoom");
    strcpy(inertia_wheels.control.lens_status, "ok. ok. ok.");
    strcpy(inertia_wheels.control.custom16_name, "Custom 16");
    strcpy(inertia_wheels.control.custom32_name, "Custom 32");
    inertia_wheels.control.battery_left = 13.1;
    inertia_wheels.control.battery_right = 11.9;
    inertia_wheels.control.battery_warning_voltage = 12.0;
}

void loop() {
    static int count;
    if (Serial1.available()){
        inertia_wheels.injest(Serial1.read());
        if (inertia_wheels.new_packet()){
            count ++;
            if (count >= 10){
                count = 0;
                inertia_wheels.build_control_packet();
                for (unsigned int i=0; i < SIZE_OF_API_REPLY; i++) {
                    Serial1.write(inertia_wheels.control_packet[i]);
                }
            }
        }
    }
}