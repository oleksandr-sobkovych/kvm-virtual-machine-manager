#include "guest.h"

#include <iostream>


int main(int argc, char *argv[]) {
    try {
        Guest guestMachine;
        guestMachine.boot(argv[1], argv[2]);
        guestMachine.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
