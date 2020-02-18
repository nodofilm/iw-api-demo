#ifndef iw_api_h
#define iw_api_h

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SIZE_OF_API_PACKET 53
#define SIZE_OF_API_BODY (SIZE_OF_API_PACKET-9)

#define SIZE_OF_API_REPLY 203
#define SIZE_OF_API_REPLY_BODY (SIZE_OF_API_REPLY-9)

#define DEBUG_IWAPI false

#define API_VERSION 1

enum _api_error_t : uint8_t {GOOD = 0, NOT_READY, WRONG_SIZE, BAD_CHECKSUM, BAD_FOOTER, WRONG_VERSION};
enum _api_status_level : uint8_t {OFF = 0, OKAY = 50, ATTENTION = 100, ERROR = 200};

struct iw_api_data_t {
    int32_t pan;
    int32_t tilt;
    int32_t roll;
    int32_t knob1;
    int32_t knob2;
    uint16_t focus;
    uint16_t iris;
    uint16_t zoom;
    int32_t rssi;
    float snr;
    uint8_t _blip_1;
    uint8_t _blip_2;
    uint8_t _blip_3;
    uint8_t _blip_4;
    uint8_t _states0;
    uint8_t _states1;
    uint8_t _reserved[8];
};
struct iw_api_knob_t {
    char name[15];
    bool ini;
    int32_t set;
    int32_t max;
    int32_t min;
    uint16_t scaling = 1;
};

struct iw_api_control_t {
    char name[9];
    char status[15];
    _api_status_level status_level;
    iw_api_knob_t knob1;
    iw_api_knob_t knob2;
    char button1_name[15];
    char button2_name[15];
    char button3_name[15];
    char button4_name[15];
    char roll_name[15];
    char lens_status[15];
    float battery_left;
    float battery_right;
    float battery_warning_voltage;
    uint8_t _reserved[8];
};
class iw_api_c {
    public:
        iw_api_data_t data;
        iw_api_control_t control;
        void injest(char in);
        bool new_packet();
        void build_control_packet();
        bool buttonPressed(uint8_t button); 
        uint8_t control_packet[SIZE_OF_API_REPLY];
    private:
        uint8_t version = 1;
        uint8_t reply_header[5] =  {'I','W','A','P','I'};
        uint8_t reply_footer[3] = {';','!',';'};
        bool new_packet_flag;
        char buffer[SIZE_OF_API_PACKET];
        unsigned short uin16_ify(char in[2]);
        int int32_ify(char in[4]);
        _api_error_t in_byte(char in);
        void parse();
        char checksum(char *buf_p, int len);
        int32_t addItem(void * to, void * from, int size, int offset);
};

#endif