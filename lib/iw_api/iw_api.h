#ifndef iw_api_h
#define iw_api_h

#define SIZE_OF_API_PACKET 51
#define SIZE_OF_API_BODY 42

#define DEBUG_IWAPI false

#define API_VERSION 1

struct iw_api_data_t {
    public:
        int pan;
        int tilt;
        int roll;
        int c1;
        unsigned short focus;
        unsigned short iris;
        unsigned short zoom;
        unsigned short c2;
        int rssi();
        float snr();
    private:
        char _blip_1;
        char _blip_2;
        char _blip_3;
        char _blip_4;
        char _states0;
        char _states1;
        char _reserved[8];
        char _rssi;
        char _snr;
};
struct iw_api_control_t {

};
enum _api_error_t {GOOD = 0, NOT_READY, WRONG_SIZE, BAD_CHECKSUM, BAD_FOOTER, WRONG_VERSION};
class iw_api_c {
    public:
        iw_api_data_t data;
        iw_api_control_t control;
        void injest(char in);
        bool new_packet();
    private:
        bool new_packet_flag;
        char buffer[SIZE_OF_API_PACKET];
        _api_error_t in_byte(char in);
        void parse();
        char checksum(char *buf_p, int len);
};

#endif