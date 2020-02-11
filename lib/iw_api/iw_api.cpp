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
    data.c1 = int32_ify(&buffer[19]);
    data.focus = uin16_ify(&buffer[23]);
    data.iris = uin16_ify(&buffer[25]);
    data.zoom = uin16_ify(&buffer[27]);
    data.c2 = uin16_ify(&buffer[29]);
    data._blip_1 = buffer[30];
    data._blip_2 = buffer[31];
    data._blip_3 = buffer[32];
    data._blip_4 = buffer[33];
    data._states0 = buffer[34];
    data._states1 = buffer[35];
    data._reserved[0] = buffer[36];
    data._reserved[1] = buffer[37];
    data._reserved[2] = buffer[38];
    data._reserved[3] = buffer[39];
    data._reserved[4] = buffer[40];
    data._reserved[5] = buffer[41];
    data._reserved[6] = buffer[42];
    data._reserved[7] = buffer[43];
    data._reserved[8] = buffer[44];
    data.rssi = buffer[45] - 128;
    data.snr = (buffer[47] - 128) / 10.0f;
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
    int i = 0;
    int size = 0;
    i = 0; 
    size =  sizeof(reply_header);
    memcpy(control_packet+i, reply_header, size);
    i += size; 
    size = sizeof(control.name);
    memcpy(control_packet+i, control.name, size);
    i += size; 
    size = sizeof(control.focus_name);
    memcpy(control_packet+i, control.focus_name, size);
    i += size; 
    size = sizeof(control.iris_name);
    memcpy(control_packet+i, control.iris_name, size);
    i += size; 
    size = sizeof(control.zoom_name);
    memcpy(control_packet+i, control.zoom_name, size);
    i += size; 
    size = sizeof(control.lens_status);
    memcpy(control_packet+i, control.lens_status, size);
    i += size; 
    size = sizeof(control.custom32_name);
    memcpy(control_packet+i, control.custom32_name, size);
    i += size; 
    size = sizeof(control.custom16_name);
    memcpy(control_packet+i, control.custom16_name, size);
    i += size; 

    size = 1;
    control_packet[i] = control._internal[0];
    i += 1;

    size = 1;
    control_packet[i] = control._internal[1];
    i += 1;

    size = sizeof(control.battery_left);
    memcpy(control_packet+i, &control.battery_left, size);
    i += size; 
    size = sizeof(control.battery_right);
    memcpy(control_packet+i, &control.battery_right, size);
    i += size; 
    size = sizeof(control.battery_warning_voltage);
    memcpy(control_packet+i, &control.battery_warning_voltage, size);
    i += size; 
    size = 1;
    memcpy(control_packet+i, &control.status_level, 1);
    i += size; 
    size = sizeof(control.status);
    memcpy(control_packet+i, control.status, size);
    i += size; 
    size = sizeof(control._reserved);
    memcpy(control_packet+i, control._reserved, size);
    
    char body[SIZE_OF_API_REPLY_BODY];
    for (int x = 0; x < SIZE_OF_API_REPLY_BODY; x++) body[x] = control_packet[x+5];  //shift the body of the packet into the body
    
    i += size;
    control_packet[i] = checksum(body, SIZE_OF_API_REPLY_BODY);
    i += 1;
    size =  sizeof(reply_footer);
    memcpy(control_packet+i, reply_footer, size);
    

    //copy out into the control_packet buffer
    // const uint8_t pkt_size = sizeof(struct iw_api_reply_t);
    // memcpy(control_packet, &reply_struct, pkt_size);
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