#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;

int main(int argc, char** argv)
{
    pid_t pid = getpid();
    string type = strcmp(argv[1], "1") == 0 ? "producer" : "consumer";
    cout << "I am " << argv[0] << " (" << type << ") My PID is " << getpid() << endl;

    return 0;
}