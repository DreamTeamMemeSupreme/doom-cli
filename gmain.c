#include "client_general.h"

// argument list is ignored now, but in future will
// expect ip address of server and its port
int main(int argc, char **argv)
{
    // preparation work and launching threads
    init_app();

    // run main loop
    exec_app();

    return 0;
}