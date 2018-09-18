#include <iostream>
//#define arduino
#if defined(arduino)
#define os_string String
#else
#define os_string std::string
#endif
using namespace std;
class Document {
public:
    virtual void transfer(const char * name, int & v){
        cout << "\"" <<  name << "\":" << v << endl;
    }
    virtual void transfer(const char * name, os_string &v){
        cout << "\"" << name << "\":" << "\"" <<  v << "\"" <<  endl;
    }
};

class Transferrable {
public:
    int myval = 3;
    os_string mystring = "hello";
    virtual void transfer(Document & doc) {
        doc.transfer("myval", myval);
        doc.transfer("mystring", mystring);
    }
};

int main() {
    Transferrable obj;
    Document doc;
    obj.transfer(doc);
    return 0;
}