// messages that will be sent
// to the outside world

#ifdef ARDUINO
    #define NativeString String
    #define to_string NativeString
#else
    #define NativeString std::string
    #define to_string std::to_string
#endif
#include <cstring>
#include <cstdlib>

class ITransfer {
public:
    virtual void transfer(NativeString &) = 0;
    virtual void transfer(int &) = 0;
    virtual void complete() {};
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
         add_field(to_string(v).c_str());
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
            throw NativeString("not enough fields");
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

    virtual void complete() {
        if(!done) {
            throw NativeString("too many fields");
        }
    }

};




struct TraceDynamics {
    int number = 0;
    NativeString label = "";
    void transfer(ITransfer & document) {
        document.transfer(number);
        document.transfer(label);
        document.complete();
    }
};
