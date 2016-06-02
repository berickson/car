#include <iostream>
#include <thread>
#include <unistd.h> // for usleep, etc

using namespace std;

class Printer {
public:
    string text;
    void run() {
        cout << text << endl;
    }
};

int main(int, char *[])
{
    Printer p;
    p.text = "calling a member from a thread";
    thread t1(&Printer::run, p);
    t1.join();
    return 0;
}
