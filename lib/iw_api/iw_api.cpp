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
    }
}


/*
Takes the universal buffer and parses it into the data 
*/
void iw_api_c::parse() {
    
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
    for (int i = 1; i < SIZE_OF_API_PACKET; i++){
        buffer[i-1] = buffer[i]; 
    }
    buffer[SIZE_OF_API_PACKET-1] = in;

    if (buffer[0] != 'I') return NOT_READY;
    if (buffer[1] != 'W') return NOT_READY;
    if (buffer[2] != 'A') return NOT_READY;
    if (buffer[3] != 'P') return NOT_READY;
    if (buffer[4] != 'I') return NOT_READY;

    if (buffer[5] != API_VERSION) return WRONG_VERSION;

    char body[SIZE_OF_API_BODY];
    for (int x = 0; x < SIZE_OF_API_BODY; x++) body[x] = buffer[x+6];  //shift the body of the packet into the body
    
    if (buffer[SIZE_OF_API_PACKET-3] != ';') return WRONG_SIZE;
    if (buffer[SIZE_OF_API_PACKET-2] != '!') return WRONG_SIZE;
    if (buffer[SIZE_OF_API_PACKET-1] != ';') return WRONG_SIZE;
    
    char cs = buffer[SIZE_OF_API_PACKET-4];
    char correct = checksum(body, SIZE_OF_API_BODY);
    if (cs != correct) return BAD_CHECKSUM;
    
    return GOOD;
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