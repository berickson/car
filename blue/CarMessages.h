// messages that will be sent
// to the outside world
#ifndef CAR_MESSAGES_H
#define CAR_MESSAGES_H

#ifdef ARDUINO
    #define NativeString String
    #define to_native_string NativeString
    String ftos(float f,int n) {
        return String(f,n);
    }
#else
    #define NativeString std::string
    #define to_native_string std::to_string
#endif
#include <cstring>
#include <cstdlib>
#include <cmath> // for NAN

class ITransfer {
public:
    virtual void transfer(float &, u_int8_t decimal_digits = 2) = 0;
    virtual void transfer(NativeString &) = 0;
    virtual void transfer(char &) = 0;
    virtual void transfer(int &) = 0;
    virtual void transfer(bool &) = 0;
    virtual void transfer(unsigned long &) = 0;
    virtual void transfer(long &) = 0;
    virtual void complete() {};
    bool ok = true;
    NativeString error_message;

};

class StringOutTransfer : public ITransfer {
    NativeString s;

    inline void add_field(const char * f) {
        if(s[0]!=0) {
            s += ",";
        }
        s += f;
    }

public:
    StringOutTransfer(size_t reserve = 500) {
        s.reserve(reserve);
    }

    NativeString str() {
        return s;
    }

    virtual void transfer(float & v, u_int8_t decimal_digits) {
        add_field(ftos((double)v, (int)decimal_digits).c_str());
    }

    virtual void transfer(NativeString & v) {
        add_field(v.c_str());
    }

    virtual void transfer(char & v) {
        NativeString s;
        s += v;
         add_field(s.c_str());
    }

    virtual void transfer(int & v) {
         add_field(to_native_string(v).c_str());
    }

    virtual void transfer(bool & v) {
        add_field(v ? "1" : "0");
    }

    virtual void transfer(unsigned long & v) {
         add_field(to_native_string(v).c_str());
    }

    virtual void transfer(long & v) {
         add_field(to_native_string(v).c_str());
    }
};


class StringInTransfer : public ITransfer {
    size_t pos = 0;
    char delim = ',';
    NativeString s;
    NativeString field;
    bool done = false;

    inline NativeString & get_field() {
        if(done) {
            ok = false;
            error_message = "not enough fields";
        }
        // find delim or end of string
        field = "";
        char c;
        while( (c = s[pos]) && c != delim) {
          field += c;
          ++pos;
        }
        if(c) {
            ++pos;
        } else {
            done = true;
        }
        return field;
    }

public:
    StringInTransfer(const char * t) {
        s = t;
        pos = 0;
        field.reserve(s.length());
    }

    virtual void transfer(float & v, u_int8_t ) {
        v = strtof(get_field().c_str(), NULL);
    }

    virtual void transfer(char & v) {
        auto f = get_field();
        if(f.length()==0) {
            v = ' ';
        } else {
            v = f[0];
        }
    }

    virtual void transfer(NativeString & v) {
        v = get_field();
    }

    virtual void transfer(int & v) {
        v = atoi(get_field().c_str());
    }

    virtual void transfer(bool & v) {
        v = (get_field() == "1");
    }
    
    virtual void transfer(long & v) {
        v = atol(get_field().c_str());
    }

    virtual void transfer(unsigned long & v) {
        v = strtoul(get_field().c_str(),NULL,10);
    }

    virtual void complete() {
        if(!done) {
            ok = false;
            error_message = "too many fields";
        }
    }

};


struct Dynamics2 {
    unsigned long ms = 0;
    unsigned long us = 0;

    float v_bat = NAN;

    NativeString mode;
    
    int rx_str = -1;
    int rx_esc = -1;


    // acceleration
    float ax = NAN;
    float ay = NAN;
    float az = NAN;


    unsigned long spur_us = 0;
    long spur_odo = 0;

    long odo_fl_a = 0;
    unsigned long odo_fl_a_us = 0;
    long odo_fl_b = 0;
    unsigned long odo_fl_b_us = 0;
    unsigned long odo_fl_ab_us = 0;

    long odo_fr_a = 0;
    unsigned long odo_fr_a_us = 0;
    long odo_fr_b = 0;
    unsigned long odo_fr_b_us = 0;
    unsigned long odo_fr_ab_us = 0;
    
    long odo_bl_a = 0;
    unsigned long odo_bl_a_us = 0;
    long odo_bl_b = 0;
    unsigned long odo_bl_b_us = 0;
    unsigned long odo_bl_ab_us = 0;
    
    long odo_br_a = 0;
    unsigned long odo_br_a_us = 0;
    long odo_br_b = 0;
    unsigned long odo_br_b_us = 0;
    unsigned long odo_br_ab_us = 0;

    float mpu_deg_yaw = NAN;
    float mpu_deg_pitch = NAN;
    float mpu_deg_roll = NAN;
    float mpu_deg_f = NAN;

    bool go = false;

    void transfer(ITransfer & document) {

        document.transfer(ms);
        document.transfer(us);

        document.transfer(v_bat);

        document.transfer(mode);

        document.transfer(rx_esc);
        document.transfer(rx_str);

        document.transfer(spur_us);
        document.transfer(spur_odo);

        // imu acceleration
        document.transfer(ax);
        document.transfer(ay);
        document.transfer(az);

        document.transfer(odo_fl_a);
        document.transfer(odo_fl_a_us);
        document.transfer(odo_fl_b);
        document.transfer(odo_fl_b_us);
        document.transfer(odo_fl_ab_us);

        document.transfer(odo_fr_a);
        document.transfer(odo_fr_a_us);
        document.transfer(odo_fr_b);
        document.transfer(odo_fr_b_us);
        document.transfer(odo_fr_ab_us);

        document.transfer(odo_bl_a);
        document.transfer(odo_bl_a_us);
        document.transfer(odo_bl_b);
        document.transfer(odo_bl_b_us);
        document.transfer(odo_bl_ab_us);

        document.transfer(odo_br_a);
        document.transfer(odo_br_a_us);
        document.transfer(odo_br_b);
        document.transfer(odo_br_b_us);
        document.transfer(odo_br_ab_us);

        document.transfer(mpu_deg_yaw);
        document.transfer(mpu_deg_pitch);
        document.transfer(mpu_deg_roll);
        document.transfer(mpu_deg_f);

        document.transfer(go);


        document.complete();
    }

};

struct SimpleMessage {
    int int_value = 0;
    NativeString string_value = "";
    char char_value = '?';
    float float_value = NAN;
    
    void transfer(ITransfer & document) {
        document.transfer(int_value);
        document.transfer(string_value);
        document.transfer(char_value);
        document.transfer(float_value);
        document.complete();
    }
};

#endif
