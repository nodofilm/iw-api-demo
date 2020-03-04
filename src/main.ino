#include <Arduino.h>
#include "iw_api.h"

    
iw_api_c inertia_wheels;                                        //Create an instance of the Inertia Wheels class

void data_print();
void knob_ini_check();
void check_sync();

void setup() {
    Serial.begin(115200);
    Serial1.begin(460800);                                      //This the required baud
    strcpy(inertia_wheels.control.name, "API Demo");            //Name your project. Up to 8 characters
    
    strcpy(inertia_wheels.control.status, "System Okay");       //Show a status on both the dashboard an in the menu. Up to 14 characters
    inertia_wheels.control.status_level = IW_OKAY;                 //Set this to change the intensity of 
    strcpy(inertia_wheels.control.lens_status, "OK  OK  OK ");  // Up to 14 characters

    //Knob 1
    strcpy(inertia_wheels.control.knob1.name, "Setting 1");     //Setting a name here, enables this knob
    inertia_wheels.control.knob1.set = 2;
    inertia_wheels.control.knob1.max = 10;
    inertia_wheels.control.knob1.min = 0;

    strcpy(inertia_wheels.control.knob2.name, "Setting 2");     //This enables this knob
    inertia_wheels.control.knob2.set = -20;                     //This value is the initial and default value available to the user. This can only be set once.
    inertia_wheels.control.knob2.max = 100;
    inertia_wheels.control.knob2.min = -100;                
    inertia_wheels.control.knob2.scaling = 10;                  //Scaling is on and set to 10

    /*
    ABOUT SCALING
    Scaling will multiply the values the user inputs AND smoothing between changes in these scaled values.
    Scaling is used because knob values are not transmitted as frequently as wheel values are. 
    Therefore, without scaling, these values would be steppy. Scaling allows smothing via interpolation.
    This is helpful for any values that will be used in real time to effect a shot.
    - If a knob does not have scaling, if the user sets a knob to 2, the API will output 2.
    - If a knob has scaling, the user will see 2, but the API will output scaling*2.
    EXAMPLE
    If you provide a range of -100 to 100. With a scaling of 10. 
    The user will see -100 to 100 available to them.
    But the range of the API output will be -1000 to 1000.
    If the user sets the knob to the max of 100, the api will output 1000 (or 10x100)
    If the user then moves the knob all the way to -100, the API will smoothly transition values 
    all the way from 1000 to -1000.
    */
    
    //User Buttons
    //If you do not name the button, it will not appear to the user
    strcpy(inertia_wheels.control.button1_name, "Button 1");
    strcpy(inertia_wheels.control.button2_name, "Button 2");
    strcpy(inertia_wheels.control.button3_name, "Button 3");
    strcpy(inertia_wheels.control.button4_name, "Button 4");

    //Battery Indicators
    //If you do not set the battery levels, they will not appear on the dashboard
    inertia_wheels.control.battery_left = 13.1;
    inertia_wheels.control.battery_right = 11.9;
    inertia_wheels.control.battery_warning_voltage = 12.0;  //This is the voltage threshold at which the display will start to warn the user that hte battery is low

    inertia_wheels.ini(); //initialize
}

void loop() {
    static int count;
    if (Serial1.available()){
        inertia_wheels.injest(Serial1.read());                              //injest() takes one byte at a time and puts it into the class, it will perform all necessary tasks to parse the incoming packet as well. 
        if (inertia_wheels.new_packet()){                                   //new_packet() returns true once a new packet has been found and parsed. New data is ready to be used by your application now.
            //Run your tasks:
            // data_print();
            check_buttons();
            check_sync();
            //Generate a control reply. You do not need to update at 100Hz. Anywhere from 1Hz to 10Hz will suffice. 
            count ++;
            if (count >= 10){                                               //This counter is used to send a control packet 1 out of ever 10 incoming packets
                count = 0;
                inertia_wheels.build_control_packet();                      //build_control_packet() takes all given control data and fills the control_packet buffer
                for (unsigned int i=0; i < SIZE_OF_API_REPLY; i++) {
                    Serial1.write(inertia_wheels.control_packet[i]);        //All bytes in the control_packet buffer must are written out the serial port
                }
            }
        }
    }
}

void data_print() {
    Serial.println("FRESH WHEEL DATA");
    Serial.println(inertia_wheels.data.pan);
    Serial.println(inertia_wheels.data.tilt);
    Serial.println(inertia_wheels.data.roll);
    Serial.println(inertia_wheels.data.knob1);
    Serial.println(inertia_wheels.data.knob2);
    Serial.println(inertia_wheels.data.focus);
    Serial.println(inertia_wheels.data.iris);
    Serial.println(inertia_wheels.data.zoom);
    Serial.println(inertia_wheels.data.rssi);
    Serial.println(inertia_wheels.data.snr);
    Serial.println();
}

void check_buttons() {
    if (inertia_wheels.buttonPressed(1)) {
        strcpy(inertia_wheels.control.name, "API 2"); 
        strcpy(inertia_wheels.control.knob1.name, "Setting 3");     //Setting a name here, enables this knob
        inertia_wheels.control.knob1.set = 5;
        inertia_wheels.control.knob1.max = 10;
        inertia_wheels.control.knob1.min = 0;             
        inertia_wheels.control.knob1.scaling = 1; 

        strcpy(inertia_wheels.control.knob2.name, "Setting 4");     //This enables this knob
        inertia_wheels.control.knob2.set = -40;                     //This value is the initial and default value available to the user. This can only be set once.
        inertia_wheels.control.knob2.max = 120;
        inertia_wheels.control.knob2.min = -120;                
        inertia_wheels.control.knob2.scaling = 20; 
        inertia_wheels.ini(); 
    }
    if (inertia_wheels.buttonPressed(2)) {
        strcpy(inertia_wheels.control.name, "API 2"); 
        strcpy(inertia_wheels.control.knob1.name, "Setting 3");     //Setting a name here, enables this knob
        inertia_wheels.control.knob1.set = 0;
        inertia_wheels.control.knob1.max = 10;
        inertia_wheels.control.knob1.min = 0;             
        inertia_wheels.control.knob1.scaling = 1; 

        strcpy(inertia_wheels.control.knob2.name, "Setting 4");     //This enables this knob
        inertia_wheels.control.knob2.set = 120;                     //This value is the initial and default value available to the user. This can only be set once.
        inertia_wheels.control.knob2.max = 120;
        inertia_wheels.control.knob2.min = -120;                
        inertia_wheels.control.knob2.scaling = 20; 
        inertia_wheels.ini();
    }
    if (inertia_wheels.buttonPressed(3)) {
        Serial.println("Button 3 Pressed");
    }
    if (inertia_wheels.buttonPressed(4)) {
        Serial.println("Button 4 Pressed");
    }
}

void check_sync() {
    if (!inertia_wheels.ini_sync()) Serial.println("out of sync");
}