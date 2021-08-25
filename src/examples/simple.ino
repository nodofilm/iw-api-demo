#include <Arduino.h>
#include "iw_api.h"

/*
SIMPLE EXAMPLE
------------------------------------
This example code demonstrates an an arduino-based MCU reading data from Inertia Wheels. 
The data being received in this example are just wheel positions. 
Focus, iris, zoom, wheel mass and speed settings, wireless signal strength are all available as read-only data as well. 
*/
    
iw_api_c inertia_wheels;                                        //Create an instance of the Inertia Wheels class

void print_wheel_data();

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    Serial.begin(9600);
    Serial1.begin(460800);                                      //This the required baud
}

void loop() {
    static int count;
    if (Serial1.available()){
        inertia_wheels.injest(Serial1.read());                              //injest() takes one byte at a time and puts it into the class, it will perform all necessary tasks to parse the incoming packet as well. 
        if (inertia_wheels.new_packet()){                                   //new_packet() returns true once a new packet has been found and parsed. New data is ready to be used by your application now.
            //Run your tasks here
            print_wheel_data();
        }
    }
}

void print_wheel_data() {
    Serial.println("WHEELS");
    Serial.println(inertia_wheels.data.pan);
    Serial.println(inertia_wheels.data.tilt);
    Serial.println(inertia_wheels.data.roll);
}







