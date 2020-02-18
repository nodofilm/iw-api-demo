#include <Arduino.h>
#include "iw_api.h"
    
iw_api_c inertia_wheels;

void data_print();
void knob_ini_check();

void setup() {
    Serial.begin(115200);
    Serial1.begin(460800);
    strcpy(inertia_wheels.control.name, "API Demo");
    strcpy(inertia_wheels.control.status, "System Okay");
    inertia_wheels.control.status_level = OKAY;
    strcpy(inertia_wheels.control.lens_status, "ok. ok. ok.");

    strcpy(inertia_wheels.control.knob1.name, "Setting 1");
    inertia_wheels.control.knob1.set = 2;
    inertia_wheels.control.knob1.max = 10;
    inertia_wheels.control.knob1.min = 0;
    inertia_wheels.control.knob1.scaling = 1;

    strcpy(inertia_wheels.control.knob2.name, "Setting 2");
    inertia_wheels.control.knob2.set = -20;
    inertia_wheels.control.knob2.max = 100;
    inertia_wheels.control.knob2.min = -100;
    inertia_wheels.control.knob2.scaling = 10;

    strcpy(inertia_wheels.control.button1_name, "Button 1");
    strcpy(inertia_wheels.control.button2_name, "Button 2");
    strcpy(inertia_wheels.control.button3_name, "Button 3");
    strcpy(inertia_wheels.control.button4_name, "Button 4");

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
            if (inertia_wheels.buttonPressed(1)) {
                Serial.println("Button 1 Pressed");
            }
            if (inertia_wheels.buttonPressed(2)) {
                Serial.println("Button 2 Pressed");
            }
            if (inertia_wheels.buttonPressed(3)) {
                Serial.println("Button 3 Pressed");
            }
            if (inertia_wheels.buttonPressed(4)) {
                Serial.println("Button 4 Pressed");
            }
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
    Serial.println(inertia_wheels.data.knob1);
    Serial.println(inertia_wheels.data.knob2);
    Serial.println(inertia_wheels.data.rssi);
    Serial.println(inertia_wheels.data.snr);
    Serial.println();
}
