#ifndef iw_api_h
#define iw_api_h

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SIZE_OF_API_PACKET 61
#define SIZE_OF_API_BODY (SIZE_OF_API_PACKET-9)

#define SIZE_OF_API_REPLY 219
#define SIZE_OF_API_REPLY_BODY (SIZE_OF_API_REPLY-9)

#define DEBUG_IWAPI true

#define API_VERSION 2

enum _api_error_t : uint8_t {IW_GOOD = 0, IW_NOT_READY, IW_WRONG_SIZE, IW_BAD_CHECKSUM, IW_BAD_FOOTER, IW_WRONG_VERSION};
enum _api_status_level : uint8_t {IW_OFF = 0, IW_OKAY = 50, IW_ATTENTION = 100, IW_ERROR = 200};

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
    uint8_t pan_mass;
    uint8_t tilt_mass;
    uint8_t pan_speed;
    uint8_t tilt_speed;
    float snr;
    uint16_t _host_sid;
    uint8_t _blip_1;
    uint8_t _blip_2;
    uint8_t _blip_3;
    uint8_t _blip_4;
    uint8_t _states_0;
    uint8_t _states_1;
    uint8_t _reserved[8];
    int32_t _pan_raw;
    int32_t _tilt_raw;
    int32_t _roll_raw;
    uint8_t _knob1_uid;
    uint8_t _knob2_uid;
    bool knob1_ready;
    bool knob2_ready;
};
struct iw_api_knob_t {
        char name[15];
        void config(int32_t set, int32_t max, int32_t min, int32_t scaling = 1);
        uint8_t _uid_setup;
        int32_t _set;
        int32_t _max;
        int32_t _min;
        uint16_t _scaling = 1;
};

struct iw_api_control_t {
    uint16_t client_sid;
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
        uint16_t host_sid;
    private:
        void new_offset(int32_t pan, int32_t tilt, int32_t roll);
        iw_api_data_t last_data;
        void packetWatchDog();
        void clearFirstButtons();
        uint8_t version = 1;
        uint8_t reply_header[5] =  {'I','W','A','P','I'};
        uint8_t reply_footer[3] = {';','!',';'};
        int32_t pan_offset;
        int32_t tilt_offset;
        int32_t roll_offset;
        bool new_packet_flag;
        char buffer[SIZE_OF_API_PACKET];
        unsigned short uin16_ify(char in[2]);
        int int32_ify(char in[4]);
        _api_error_t in_byte(char in);
        void parse();
        char checksum(char *buf_p, int len);
        int32_t addItem(void * to, void * from, int size, int offset);
        int32_t saved_pan, saved_tilt, saved_roll;
        bool packet_session_match = false;
};

#endif