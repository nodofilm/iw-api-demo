#include <Arduino.h>
#include "iw_api.h"

/*
INTERMEDIATE EXAMPLE
------------------------------------
This example code demonstrates an an arduino-based MCU reading data from Inertia Wheels and sending basic data back to the Wheels. 
The data being received includes wheel positions, focus, iris, zoom, wheel mass and speed settings, wireless signal strength. 
The data being sent back, includes a name, system status, and battery levels. 
*/
    
iw_api_c inertia_wheels;                                        //Create an instance of the Inertia Wheels class

void print_wheel_data();
void print_fiz_data();
void print_signal_data();
void print_mass_data();
void knob_ini_check();

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    Serial.begin(9600);
    Serial1.begin(460800);                                      //This the required baud
    strcpy(inertia_wheels.control.name, "API Demo");            //Name your project. Up to 8 characters
    
    strcpy(inertia_wheels.control.status, "System Okay");       //Show a status on both the dashboard an in the menu. Up to 14 characters
    inertia_wheels.control.status_level = IW_OKAY;                 //Set this to change the intensity of 
    
    //Battery Indicators
    //If you do not set the battery levels, they will not appear on the dashboard
    inertia_wheels.control.battery_left = 13.1;
    inertia_wheels.control.battery_right = 11.9;
    inertia_wheels.control.battery_warning_voltage = 12.0;  //This is the voltage threshold at which the display will start to warn the user that hte battery is low

}

void loop() {
    static int count;
    if (Serial1.available()){
        inertia_wheels.injest(Serial1.read());                              //injest() takes one byte at a time and puts it into the class, it will perform all necessary tasks to parse the incoming packet as well. 
        if (inertia_wheels.new_packet()){                                   //new_packet() returns true once a new packet has been found and parsed. New data is ready to be used by your application now.
            
            //Run your tasks here
            print_wheel_data();
            print_fiz_data();
            print_signal_data();
            print_mass_data();

            //Generate a control reply. You do not need to update at 100Hz. Anywhere from 1Hz to 10Hz will suffice. 
            count ++;
            if (count >= 10){                                               //This counter is used to send a control packet 1 out of ever 10 incoming packets
                count = 0;
                inertia_wheels.build_control_packet();                      //build_control_packet() takes all given control data and fills the control_packet buffer
                for (unsigned int i=0; i < SIZE_OF_API_REPLY; i++) {
                    Serial1.write(inertia_wheels.control_packet[i]);        //All bytes in the control_packet buffer must are written out the serial port

                    // Serial.print((char) inertia_wheels.control_packet[i]);
                    // Serial.print("_");
                }

            }
        }
    }
    // static elapsedMillis timer;
    // if (timer > 1000){
    //     Serial.println("Alive!");
    //     timer = 0;
    // }
}

void print_wheel_data() {
    Serial.println("WHEELS");
    Serial.println(inertia_wheels.data.pan);
    Serial.println(inertia_wheels.data.tilt);
    Serial.println(inertia_wheels.data.roll);
}

void print_fiz_data() {
    Serial.println("FIZ");
    Serial.println(inertia_wheels.data.focus);
    Serial.println(inertia_wheels.data.iris);
    Serial.println(inertia_wheels.data.zoom);
}

void print_signal_data(){
    Serial.print("Session ID: ");
    Serial.println(inertia_wheels.data._session_id);
    Serial.print("RSSI: ");
    Serial.println(inertia_wheels.data.rssi);
    Serial.print("SNR: ");
    Serial.println(inertia_wheels.data.snr);
}
void print_mass_data(){
    //You can use this information to predict what sort of movements the operator may make. Heavy mass settings will indicate slower accelerations.
    Serial.println("MASS");
    Serial.print("P Mass:");
    Serial.println(inertia_wheels.data.pan_mass);
    Serial.print("P Speed:");
    Serial.println(inertia_wheels.data.pan_speed);
    Serial.print("T Mass:");
    Serial.println(inertia_wheels.data.tilt_mass);
    Serial.print("T Speed:");
    Serial.println(inertia_wheels.data.tilt_speed);
}


