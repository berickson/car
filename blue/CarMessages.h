// messages that will be sent
// to the outside world

#ifdef ARDUINO
    #define NativeString String
    #define to_native_string NativeString
#else
    #define NativeString std::string
    #define to_native_string std::to_string
#endif
#include <cstring>
#include <cstdlib>

class ITransfer {
public:
    virtual void transfer(NativeString &) = 0;
    virtual void transfer(int &) = 0;
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
    virtual void transfer(NativeString & v) {
        add_field(v.c_str());
    }

    virtual void transfer(int & v) {
         add_field(to_native_string(v).c_str());
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

    virtual void transfer(NativeString & v) {
        v = get_field();
    }

    virtual void transfer(int & v) {
        v = atoi(get_field().c_str());
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


struct TraceDynamics {
    long odo_fl_a;
    unsigned long odo_fl_a_us;
    long odo_fl_b;
    unsigned long odo_fl_b_us;
    unsigned long odo_fl_ab_us;

    long odo_fr_a;
    unsigned long odo_fr_a_us;
    long odo_fr_b;
    unsigned long odo_fr_b_us;
    unsigned long odo_fr_ab_us;
    
    long odo_bl_a;
    unsigned long odo_bl_a_us;
    long odo_bl_b;
    unsigned long odo_bl_b_us;
    unsigned long odo_bl_ab_us;
    
    long odo_br_a;
    unsigned long odo_br_a_us;
    long odo_br_b;
    unsigned long odo_br_b_us;
    unsigned long odo_br_ab_us;

    bool go;

    void transfer(ITransfer & document) {
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


        document.complete();
    }

};

struct SimpleMessage {
    int number = 0;
    NativeString label = "";
    
    void transfer(ITransfer & document) {
        document.transfer(number);
        document.transfer(label);
        document.complete();
    }
};
