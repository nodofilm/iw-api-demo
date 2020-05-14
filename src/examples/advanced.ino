#include <Arduino.h>
#include "iw_api.h"

/*
ADVANCED EXAMPLE
------------------------------------
This example code demonstrates an an arduino-based MCU reading data from Inertia Wheels, sending status data back to the wheels, and controlling UI elements in the wheels. 
The data being received includes wheel positions, focus, iris, zoom, wheel mass and speed settings, wireless signal strength. 
The data being sent back, includes a name, system status, and battery levels. 
The UI elements being controlled are 4 customs buttons and 2 custom knobs.
*/
iw_api_c inertia_wheels;                                        //Create an instance of the Inertia Wheels class

void print_wheel_data();
void print_fiz_data();
void print_knob_data();
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
    strcpy(inertia_wheels.control.lens_status, "OK  OK  OK ");  // Up to 14 characters

    //Knob 1
    strcpy(inertia_wheels.control.knob1.name, "Setting 1");     //Setting a name here, enables this knob
    inertia_wheels.control.knob1.config(2, 10, 0);

    strcpy(inertia_wheels.control.knob2.name, "Setting 2");     //This enables this knob
    inertia_wheels.control.knob2.config(-20, 100, -100, 10);  //Scaling is on and set to 10

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
    strcpy(inertia_wheels.control.button1_name, "Buttn 1");
    strcpy(inertia_wheels.control.button2_name, "Buttn 2");
    strcpy(inertia_wheels.control.button3_name, "Buttn 3");
    strcpy(inertia_wheels.control.button4_name, "Buttn 4");

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
            
            //Example of how to request a new session at first packet. 
            static bool first_packet = true;
            if (first_packet){
                if (inertia_wheels.data._session_id != 0) {
                    inertia_wheels.request_new_session();
                }
                first_packet = false;
            }
            //Run your tasks here
            print_wheel_data();
            print_fiz_data();
            print_knob_data();
            print_signal_data();
            print_mass_data();
            check_buttons();

            //Example of firing a command when a new session is detected.
            if (inertia_wheels.new_session()) {
                Serial.println("NEW SESSION DETECTED!");
                Serial.println(inertia_wheels.data._session_id);
            }
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
void print_knob_data(){
    if (!inertia_wheels.data.knob1_ready) { //This checks to confirm the knob settings have propagated yet throught the wireless
        Serial.println("K1: ---");
    } else {
        Serial.print("K1: ");
        Serial.println(inertia_wheels.data.knob1);
    }
    if (!inertia_wheels.data.knob2_ready) { //This checks to confirm the knob settings have propagated yet throught the wireless
        Serial.println("K2: ---");
    } else {
        Serial.print("K2: ");
        Serial.println(inertia_wheels.data.knob2);    
    }
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
void check_buttons() {
    if (inertia_wheels.buttonPressed(1)) {
        strcpy(inertia_wheels.control.name, "API 2"); 
        strcpy(inertia_wheels.control.status, "But 1 Down");       //Show a status on both the dashboard an in the menu. Up to 14 characters
        inertia_wheels.control.status_level = IW_OKAY;

        strcpy(inertia_wheels.control.knob1.name, "Setting 3");     //Setting a name here, enables this knob
        inertia_wheels.control.knob1.config(5,10,0);

        strcpy(inertia_wheels.control.knob2.name, "Setting 4");     //This enables this knob
        inertia_wheels.control.knob2.config(-40,120,-120,20);
    }
    if (inertia_wheels.buttonPressed(2)) {
        strcpy(inertia_wheels.control.name, "API 2");
        inertia_wheels.control.status_level = IW_OFF; //Makes the status text invisible on the dashboard

        strcpy(inertia_wheels.control.knob1.name, "Setting 3");     //Setting a name here, enables this knob
        inertia_wheels.control.knob1.config(0,10,0,2);

        strcpy(inertia_wheels.control.knob2.name, "Setting 4");     //This enables this knob
        inertia_wheels.control.knob2.config(120,120,-120);
    }
    if (inertia_wheels.buttonPressed(3)) {
        Serial.println("Button 3 Pressed");
        strcpy(inertia_wheels.control.status, "But 3 Down");       //Show a status on both the dashboard an in the menu. Up to 14 characters
        inertia_wheels.control.status_level = IW_ATTENTION; 
    }
    if (inertia_wheels.buttonPressed(4)) {
        Serial.println("Button 4 Pressed");
        strcpy(inertia_wheels.control.status, "But 4 Down");       //Show a status on both the dashboard an in the menu. Up to 14 characters
        inertia_wheels.control.status_level = IW_ERROR; 
    }
}

