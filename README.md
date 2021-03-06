# iw-api-demo
Inertia Wheels API demo embedded applications. This project is for anyone looking to create a project/product that integrates with the Inertia Wheels. 

### About
The project contains an Arduino INO project file and a 

#### Control your Project with Inertia Wheels
Pan, Tilt, Roll, Focus, Iris, Zoom, Camera Run/Stop data are all available.

#### Control Inertia Wheels with your Project
In order to allow seemless project integration, the Inertia Wheels API has several data slots that are transmitted from your end project back to the Inertia Wheels. Battery data, naming information, status reporting, 4 definable user buttons and 2 knobs, are all accessible on the Inertia Wheels.

# iw_api_c API Library
In the lib folder, the iw_api.h/cpp files contain the iw_api_c class library. If you use this library, you can both receive data from the wheels and transmit data back to the wheels. The library does not manage sending and receiving UART signals. It will parse the API packets and make the data available to your application. 

### Initializing the library
`iw_api_c` 
Create an instance of the iw_api_c class. For all examples, we we use the instance of:
`iw_api_c inertia wheels`

### Receiving Data
`inertia_wheels.injest()`
When bytes are availble from your driver, pass them into the parser with the injest() function. This function will flow the bytes through a buffer. When the buffer is properly filled with a packet, it will then pass the packet to get parsed. After parsing, it will set a flag that a new packet is available. 

`inertia_wheels.new_packet()`
Once the packet has been parsed and data has been populated, the new_packet() function will return true. It will return true only once per packet. 

`inertia_wheels.data`
The data struct contains most of the data available that comes *from* the wheels. 

Data Object | Type | Description | Note
------------ | ------------- | ------- | ------
data.pan | int32_t | Pan position | See section on proper scaling
data.tilt | int32_t | Tilt position | Must be scaled
data.roll | int32_t | Roll position | Must be scaled
data.knob1 | int32_t | Custom API Knob Data | This can be configured
data.knob2 | int32_t | Custom API Knob Data | This can be configured
data.focus | uint16_t | Focus position | 0 - 0xFFFF Scaling
data.iris | uint16_t | Iris position | 0 - 0xFFFF Scaling
data.zoom | uint16_t | Zoom position | 0 - 0xFFFF Scaling
data.rssi | int32_t | Radio Signal Strength Indicator | In dBm
data.snr | float | Signal to Noise Ratio | *
data.\_blip_# | uint8_t | Container for button pressing | Use the buttonPressed(#) function
data.\_states_# | uint8_t\[2] | Reserved
data.\_reserved | uint8_t\[8] | Reserved

#### Scaling Wheel Data
The wheel data from pan/tilt/roll will come in as raw int32_t variables. These values should be interpreted thus that 0-327,680 is one full 360?? rotation of the camera. 

#### Button Presses
`inertia_wheels.buttonPressed(#)`
Use this bool function to check if a button is pressed. It will return true once if a button is pressed. Values are 1-4. There is no `inertia_wheels.buttonPressed(0)` 


### Sending data
Sending data back to the Inertia Wheels is *not* necessary. But doing so, will allow the users to exercise more specific control and view feedback from your system. 

`inertia_wheels.control`
This struct contains everything that be configured and controlled on the user interface on the Inertia Wheels

Control Object | Type | Description | Visibility | Note | Dynamically Updateable |
------------ | ------------- | ------- | ------ | ----- | ---
control.name | char string | Name your application | On the dashboard, in the menu | Maximum 8 characters long + null term | Yes
control.status | char string | Status reporting | On the dashboard, in the menu | Maximum 14 characters long + null term | Yes
control.status_level | uint8_t \_api_status_level | Set intensity of status report | On the dashboard | * | Yes
control.knob#.name | char string | Name/set visible the knob | In menu. User can add to dashboard | Must set all values | Yes
control.knob#.set | int32_t | Set the initial value of the knob | | Fixed, the first control packet sent will define this | No
control.knob#.max | int32_t | Maximum value available to the user | | Fixed, the first control packet sent will define this | No
control.knob#.min | int32_t | Minimum value available to the user | | Fixed, the first control packet sent will define this | No
control.knob#.scaling | uint16_t | Value > 1 will turn on scaling/smoothing | | Max/min values will be scaled by this | No
control.button#\_name | char string | Name/set visible the button | In menu. User can add to dashboard | Maximum 14 characters long + null term | Yes
control.roll\_name | char string | Rename Roll | | Maximum 14 characters long + null term | No
control.lens\_status | char string | Lens status string | | Maximum 14 characters long + null term | Yes
control.battery\_left | float | Voltage of left battery | | | Yes
control.battery\_right | float | Voltage of right battery | | | Yes
control.battery\_warning\_voltage | float | Voltage threshold for warning | | If either battery goes below this voltage, it will flash | Yes
control.\_reserved | uint8_t[8] | Reserved || | -

#### First packet
Items marked No under Dynamically Updatable are not able to be changed after the first control_packet is received at the Wheels. Be certain to set these properly during initialization. 

#### Char String Lengths
The API contains no routine to handle character strings that are too large for their container. Overpopulating these fields will result in unpredictable behavior. 

#### Order of Operations: Sending
1. First populate all fields and data. Pay special attention to be certain that items that are not dynamically updatable have been configured correctly.
2. Call `inertia_wheels.build_control_packet()` to have the library populate the `inertia_wheels.control_packet` array.
3. Send the `inertia_wheels.control_packet` out your UART buffer.

# Hardware Layer
You can communicate with the Inertia Wheels via several ports.
![Imgur](https://imgur.com/lKUdGTy.jpg)
### Receiver UART
The main output connector for embedded applications is the recievers 4-Pin output connector. 

#### UART Baud Settings
460800 Baud 8N1

#### Connector P/N
Molex P/N 105307-1204 

#### Pin Out
Pin # | Function | Notes
------|----------|-----
Pin 1 | Receiver TX -> Device RX | 3.3v Logic Level
Pin 2 | Receiver RX <- Device TX | 3.3v, 5v Tolerant
Pin 3 | VCC | 5-12v, ~0.5amp @ 5v
Pin 4 | Ground | 

### Receiver USB
The same API is sent via the USB connector on the receiver. Unfortunately as of 0.3.1, the receiver is not capable of receiving the full bi-directional API. The USB can only output wheel data. 

#### USB Baud Settings
921600 Baud 8N1


