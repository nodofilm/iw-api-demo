#include "iw_api.h"
#include "Arduino.h"

/*
Injest new UART characters here. If the packet is complete, it will flag the packet as new
*/
void iw_api_c::injest(char in) {
    _api_error_t status = in_byte(in);
    if (status == GOOD) parse();
    else if (status == NOT_READY) return;
    else {
        if (status == WRONG_VERSION) Serial.println("WRONG VERSION");
        else if (status == WRONG_VERSION) Serial.println("WRONG SIZE");
        else if (status == BAD_CHECKSUM) Serial.println("BAD CHECKSUM");
    }
}


/*
Takes the universal buffer and parses it into the data 
*/
void iw_api_c::parse() {
    data.pan = int32_ify(&buffer[7]);
    data.tilt = int32_ify(&buffer[11]);
    data.roll = int32_ify(&buffer[15]);
    data.knob1 = int32_ify(&buffer[19]);
    data.knob2 = int32_ify(&buffer[23]);
    data.focus = uin16_ify(&buffer[27]);
    data.iris = uin16_ify(&buffer[29]);
    data.zoom = uin16_ify(&buffer[31]);
    data._blip_1 = buffer[33];
    data._blip_2 = buffer[34];
    data._blip_3 = buffer[35];
    data._blip_4 = buffer[36];
    data._states0 = buffer[37];
    data._states1 = buffer[38];
    data._reserved[0] = buffer[39];
    data._reserved[1] = buffer[40];
    data._reserved[2] = buffer[41];
    data._reserved[3] = buffer[42];
    data._reserved[4] = buffer[43];
    data._reserved[5] = buffer[44];
    data._reserved[6] = buffer[45];
    data._reserved[7] = buffer[46];
    data.rssi = buffer[47] - 128;
    data.snr = (buffer[48] - 128) / 10.0f;
    new_packet_flag = true;
}


/*
Will return true, only once, if a new packet is available.
*/
bool iw_api_c::new_packet(){
    if (new_packet_flag) {
        new_packet_flag = false;
        return true;
    }
    return false;
}


/*
Receives new bytes and returns true if the byte is ready
*/
_api_error_t iw_api_c::in_byte(char in) {
    for (int i = 1; i < SIZE_OF_API_PACKET; i++)buffer[i-1] = buffer[i]; //shift buffer

    buffer[SIZE_OF_API_PACKET-1] = in; //add the new byte

    //scan for header
    if (buffer[0] != 'I') return NOT_READY;
    if (buffer[1] != 'W') return NOT_READY;
    if (buffer[2] != 'A') return NOT_READY;
    if (buffer[3] != 'P') return NOT_READY;
    if (buffer[4] != 'I') return NOT_READY;

    //scan for version
    if (buffer[5] != API_VERSION) return WRONG_VERSION;
    
    //check for proper termination
    if (buffer[SIZE_OF_API_PACKET-3] != ';') return WRONG_SIZE;
    if (buffer[SIZE_OF_API_PACKET-2] != '!') return WRONG_SIZE;
    if (buffer[SIZE_OF_API_PACKET-1] != ';') return WRONG_SIZE;

    //calculate payload checksum
    char body[SIZE_OF_API_BODY];
    for (int x = 0; x < SIZE_OF_API_BODY; x++) body[x] = buffer[x+5];  //shift the body of the packet into the body
    char cs = buffer[SIZE_OF_API_PACKET-4];
    char correct = checksum(body, SIZE_OF_API_BODY);
    if (cs != correct) return BAD_CHECKSUM;

    return GOOD;
}

//converts 2 chars into 1 32-bit unsigned short integer
unsigned short iw_api_c::uin16_ify(char in[2]){
    return ((in[0] << 8) ^ in[1]);
}

//converts 4 chars into 1 signed 32-bit integer
int iw_api_c::int32_ify(char in[4]){
    return ((in[0] << 24) ^ (in[1] << 16) ^ (in[2] << 8) ^ in[3]);
}

/*
Call to the iw_api lib to build a packet for reply and store it in the control_packet buffer to be sent over Serial
*/
void iw_api_c::build_control_packet() {
    int offset = 0; 
    offset = addItem(control_packet, reply_header, sizeof(reply_header), offset);
    offset = addItem(control_packet, &version, sizeof(version), offset);

    offset = addItem(control_packet, control.name, sizeof(control.name), offset);

    offset = addItem(control_packet, &control.status_level, sizeof(control.status_level), offset);
    offset = addItem(control_packet, control.status, sizeof(control.status), offset);

    offset = addItem(control_packet, control.knob1.name, sizeof(control.knob1.name), offset);
    offset = addItem(control_packet, &control.knob1.set, sizeof(control.knob1.set), offset);
    offset = addItem(control_packet, &control.knob1.max, sizeof(control.knob1.max), offset);
    offset = addItem(control_packet, &control.knob1.min, sizeof(control.knob1.min), offset);
    offset = addItem(control_packet, &control.knob1.scaling, sizeof(control.knob1.scaling), offset);

    offset = addItem(control_packet, control.knob2.name, sizeof(control.knob2.name), offset);
    offset = addItem(control_packet, &control.knob2.set, sizeof(control.knob2.set), offset);
    offset = addItem(control_packet, &control.knob2.max, sizeof(control.knob2.max), offset);
    offset = addItem(control_packet, &control.knob2.min, sizeof(control.knob2.min), offset);
    offset = addItem(control_packet, &control.knob2.scaling, sizeof(control.knob2.scaling), offset);

    offset = addItem(control_packet, control.button1_name, sizeof(control.button1_name), offset);
    offset = addItem(control_packet, control.button2_name, sizeof(control.button2_name), offset);
    offset = addItem(control_packet, control.button3_name, sizeof(control.button3_name), offset);
    offset = addItem(control_packet, control.button4_name, sizeof(control.button4_name), offset);

    offset = addItem(control_packet, control.roll_name, sizeof(control.roll_name), offset);
    offset = addItem(control_packet, control.lens_status, sizeof(control.lens_status), offset);

    offset = addItem(control_packet, &control.battery_left, sizeof(control.battery_left), offset);
    offset = addItem(control_packet, &control.battery_right, sizeof(control.battery_right), offset);
    offset = addItem(control_packet, &control.battery_warning_voltage, sizeof(control.battery_warning_voltage), offset);
    
    offset = addItem(control_packet, control._reserved, sizeof(control._reserved), offset);

    char body[SIZE_OF_API_REPLY_BODY];
    for (int x = 0; x < SIZE_OF_API_REPLY_BODY; x++) body[x] = control_packet[x+5];  //shift the body of the packet into the body
    uint8_t cs = checksum(body, SIZE_OF_API_REPLY_BODY);
    offset = addItem(control_packet, &cs, sizeof(cs), offset);
    
    addItem(control_packet, reply_footer, sizeof(reply_footer), offset);
}
/*
A variation of memcpy for copying multiple items into a buffer. Returns the next byte available for writing.
*/
int32_t iw_api_c::addItem(void * to, void * from, int size, int offset){
    memcpy(to+offset, from, size);
    return offset+size;
}
/*
Returns true if the button called was pressed. Pass 1-4 as the button number. There is no button 0.
*/
bool iw_api_c::buttonPressed(uint8_t num){
    static uint8_t buttons_last[5];
    if (num == 1){
        if (data._blip_1 != buttons_last[1]) {
            buttons_last[1] = data._blip_1;
            return true;
        }
    } else if (num == 2){
        if (data._blip_2 != buttons_last[2]) {
            buttons_last[2] = data._blip_2;
            return true;
        }
    } else if (num == 3){
        if (data._blip_3 != buttons_last[3]) {
            buttons_last[3] = data._blip_3;
            return true;
        }
    } else if (num == 4){
        if (data._blip_4 != buttons_last[4]) {
            buttons_last[4] = data._blip_4;
            return true;
        }
    }
    return false;
}

/*
CheckSum8 Modulo 256 
Sum of Bytes % 256
*/
char iw_api_c::checksum(char *buf_p, int len) {
    int j;
    char checksum = 0;
    for(j = 0; j < len; j++) {
        checksum += *buf_p++;
    }
    return checksum;
}