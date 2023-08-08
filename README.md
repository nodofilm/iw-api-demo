# iw-api-demo
Inertia Wheels API demo embedded applications. This project is for anyone looking to create a project/product that integrates with the Inertia Wheels.

For hardware requirements such as pin out, see [the hardware repository](https://github.com/nodofilm/iw-hardware).

### Compatibility
This version (v2) of the API is functional as of V1.0.0.

### About
The project contains an Arduino INO project file in the src folder and a minimal C++ library in the lib folder.

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
data.pan_mass | uint8_t | Pan mass 0-10 | * See Note Below
data.tilt_mass | uint8_t | Tilt mass 0-10 | * See Note Below
data.pan_speed | uint8_t | Pan speed 0-255 | * See Note Below
data.tilt_speed | uint8_t | Tilt speed 0-255 | * See Note Below
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

**Mass and Speed are sent as helpful indicators of the style of operating that the user may use. A high mass indicates they will likely use slow accelerations. A low speed (interpreted as a gear ratio) indicates that faster motor speeds may be used.*

#### Scaling Wheel Data
The wheel data from pan/tilt/roll will come in as raw int32_t variables. These values should be interpreted thus that 0-360,000 is one full 360° rotation of the camera. 

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
control.knob#.control() | function | See below for knob configuration | | | Yes
control.button#\_name | char string | Name/set visible the button | In menu. User can add to dashboard | Maximum 14 characters long + null term | Yes
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

<<<<<<< HEAD
# Knob Control
To configure a knob, you must do three things. Name the knob, configure the knob, and check if it is ready. 

#### Knob Naming
In this example, the knob is named "Set 1". By giving the knob a name, it is given visibility.
```c++
strcpy(inertia_wheels.control.knob1.name, "Set 1");
```

#### Knob Configuration
In this example, the knob will be updated on the dashboard is set to a value of -20. The knob has limits of a maximum of 100, minimum of -100. And it has a scaling factor of x10. Scaling means that in the API, the maximum values will be x10 the max, or 1000. Min x10, or -1000. Scaling allows the values to be smoothly updated with each packet. 
```c++
/*
void config(int32_t set, int32_t max, int32_t min, int32_t scaling = 1);
Configure a knob by calling config() function once.
*/
interia_wheels.control.knob1.config(-20, 100, -100, 10);
```

#### Knob Ready
Because of the delays caused by full duplex wireless system, it is imporant to use the knob#_ready bool to confirm if the desired configuration of the knob has fully propogated through the system. Without confirming the knob is ready, your application could sample from a previous configuration. Typically, the knob is ready less than 0.5 seconds after configuration.
```c++
if (inertia_wheels.data.knob1_ready) dosomething = inertia_wheels.data.knob1;
```

# Session Control
If you are building a complex bi-directional control application. For optimal smooth user experience, it may be necessary to generate a session ID. This is most helpful when the end device is rebooted and needs to start fresh.

#### Client_SID
The Client SID is sent to the Inertia Wheels. This unique identifier serves to tell the wheels that a new session has been created and to reset parameters of the API. If you processor is capable of truly random numbers. This example serves well.
```c++
inertia_wheels.control.client_sid = random(65536);
```

If your system is not capable of generating random numbers. This example shows how you could increment the client_sid and save it in your system's persistent memory for the next boot.
```c++
sid = load_sid_from_memory();
sid++;
inertia_wheels.control.client_sid = sid;
save_sid_to_memory(sid);
```
=======
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
Pin 1 | Receiver TX -> Device RX | 5.0v Logic Level
Pin 2 | Receiver RX <- Device TX | 3.3v, 5v Tolerant
Pin 3 | VCC | 5-12v, ~0.5amp @ 5v
Pin 4 | Ground | 

### Receiver USB
The same API is sent via the USB connector on the receiver. Unfortunately as of 0.3.1, the receiver is not capable of receiving the full bi-directional API. The USB can only output wheel data. 

#### USB Baud Settings
921600 Baud 8N1

## Packet Format

### Endianness

Data is sent as Big-Endian. Example, an int32_t made of the bytes arriving in the order of 0A 0B 0C 0D is equal to 0x0A0B0C0D = 168496141.

### Uint8_t Blips

The blip items are tied to buttons. Each time the button is pressed, the integer increases by 1. To follow button presses, you will need to track this value and when it changes, act on the change.

### Packet

| Byte | Function | Format                | Example   | Hex | Notes  |
|------|----------|-----------------------|-----------|-----|---|
| 0    | Header   | ASCII Char            | I         | 49  |   |
| 1    | Header   | ASCII Char            | W         | 57  |   |
| 2    | Header   | ASCII Char            | A         | 41  |   |
| 3    | Header   | ASCII Char            | P         | 50  |   |
| 4    | Header   | ASCII Char            | I         | 49  |   |
| 5    | Header   | ASCII Char            | x         | 78  |   |
| 6    | Version  | uint8_t               | 1         | 1   |   |
| 7    | Packet ID  | uint8_t               | 1         | 1   | += 1 every packet  |
| 8    | Pan      | int32_t               |           | 0A  |   |
| 9    | Pan      | int32_t               |           | 0B  |   |
| 10    | Pan      | int32_t               |           | 0C  |   |
| 11   | Pan      | int32_t               | 168496141 | 0D  |   |
| 12   | Tilt     | int32_t               |           | 0A  |   |
| 13   | Tilt     | int32_t               |           | 0B  |   |
| 14   | Tilt     | int32_t               |           | 0C  |   |
| 15   | Tilt     | int32_t               | 168496141 | 0D  |   |
| 16   | Roll     | int32_t               |           | 0A  |   |
| 17   | Roll     | int32_t               |           | 0B  |   |
| 18   | Roll     | int32_t               |           | 0C  |   |
| 19   | Roll     | int32_t               | 168496141 | 0D  |   |
| 20   | Jog A    | int32_t               |           | 0A  |   |
| 21   | Jog A    | int32_t               |           | 0B  |   |
| 22   | Jog A    | int32_t               |           | 0C  |   |
| 23   | Jog A    | int32_t               | 168496141 | 0D  |   |
| 24   | Jog B    | int32_t               |           | 0A  |   |
| 25   | Jog B    | int32_t               |           | 0B  |   |
| 26   | Jog B    | int32_t               |           | 0C  |   |
| 27   | Jog B    | int32_t               | 168496141 | 0D  |   |
| 28   | Focus    | uint16_t              |           | 0A  |   |
| 29   | Focus    | uint16_t              | 2571      | 0B  |   |
| 30   | Iris     | uint16_t              |           | 0A  |   |
| 31   | Iris     | uint16_t              | 2571      | 0B  |   |
| 32   | Zoom     | uint16_t              |           | 0A  |   |
| 33   | Zoom     | uint16_t              | 2571      | 0B  |   |
| 34   | Pan Mass   | uint8_t               | 10        | 0A  |   |
| 35   | Tilt Mass   | uint8_t               | 10        | 0A  |   |
| 36   | Pan Speed   | uint8_t               | 10        | 0A  | 1:10 Speed Ratio |
| 37   | Tilt Speed   | uint8_t               | 10        | 0A  | 1:10 Speed Ratio |
| 38   | API Button 1   | uint8_t (blip)               | 10        | 0A  | All buttons must be enumerated to appear on the wheels to the user  |
| 39   | API Button 2  | uint8_t (blip)               | 10        | 0A  |   |
| 40   | API Button 3    | uint8_t (blip)               | 10        | 0A  |   |
| 41   | API Button 4  | uint8_t (blip)              | 10        | 0A  |   |
| 42   | States 0 | uint8_t       | 10000000        | 80  | 80 = 10000000 = Recording, 0x00 = 00000000 = Not Recording |
| 43   | States 1 | uint8_t         |         | 00  | 80 = 10000000 = Recording, 0x00 = 00000000 = Not Recording |
| 44   | Reserved |                       |           |     |   |
| 45   | Reserved |                       |           |     |   |
| 46   | Reserved |                       |           |     |   |
| 47   | Reserved |                       |           |     |   |
| 48   | Reserved |                       |           |     |   |
| 49   | Reserved |                       |           |     |   |
| 50   | Reserved |                       |           |     |   |
| 51   | Reserved |                       |           |     |   |
| 52   | Session ID    | uint16_t              |           | 0A  |   |
| 53   | Session ID    | uint16_t              | 2571      | 0B  |   |
| 54   | API Knob 1 UID | uint8_t         |  10       | 0A  | Increments every time the knob is (re)initialized |
| 55   | API Knob 2 UID | uint8_t         |  10       | 0A  |  |
| 56   | RSSI | uint8_t         |  88 (-128 = -40dBm)       | 58  | Subtract 128 for true SNR |
| 57   | SNR | uint8_t         |  132 (-128 = 4SNR)      | 84  | Subtract 128 for true RSSI |
| 58   | Checksum | CheckSum8 Modulo 256  |           |     |   |
| 59   | Footer   | ASCII Char            | ;         | 3B  |   |
| 60   | Footer   | ASCII Char            | !         | 21  |   |
| 61   | Footer   | ASCII Char            | ;         | 3B  |   |


>>>>>>> master
