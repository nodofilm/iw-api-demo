#ifndef iw_api_h
#define iw_api_h
#include "Arduino.h"

#define SIZE_OF_API_PACKET 51
#define SIZE_OF_API_BODY 42

#define SIZE_OF_API_REPLY 116
#define SIZE_OF_API_REPLY_BODY 101

#define DEBUG_IWAPI false

#define API_VERSION 1

enum _api_error_t {GOOD = 0, NOT_READY, WRONG_SIZE, BAD_CHECKSUM, BAD_FOOTER, WRONG_VERSION};
enum _api_status_level {OFF = 0, OKAY = 50, ATTENTION = 100, ERROR = 200};

struct iw_api_data_t {
    int pan;
    int tilt;
    int roll;
    int c1;
    unsigned short focus;
    unsigned short iris;
    unsigned short zoom;
    unsigned short c2;
    int rssi;
    float snr;
    char _blip_1;
    char _blip_2;
    char _blip_3;
    char _blip_4;
    char _states0;
    char _states1;
    char _reserved[8];
};

struct iw_api_control_t {
    char name[9];
    char status[15];
    _api_status_level status_level;
    char lens_status[15];
    char focus_name[9];
    char iris_name[9];
    char zoom_name[9];
    char custom32_name[9];
    char custom16_name[9];
    float battery_left;
    float battery_right;
    float battery_warning_voltage;
    uint8_t _reserved[8];
    uint8_t _internal[2];
};
class iw_api_c {
    public:
        iw_api_data_t data;
        iw_api_control_t control;
        void injest(char in);
        bool new_packet();
        void build_control_packet();
        uint8_t control_packet[SIZE_OF_API_REPLY];
    private:
        uint8_t breakout[200];
        uint8_t reply_header[5] =  {'I','W','A','P','I'};
        uint8_t reply_footer[3] = {';','!',';'};
        bool new_packet_flag;
        char buffer[SIZE_OF_API_PACKET];
        unsigned short uin16_ify(char in[2]);
        int int32_ify(char in[4]);
        _api_error_t in_byte(char in);
        void parse();
        char checksum(char *buf_p, int len);
};

#endif