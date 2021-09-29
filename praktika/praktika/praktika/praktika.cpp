#include <iostream>
#include "Extract.h"

int main(int argc, char* argv[]) {
    // primer
    Result q = functionInformation("getaddrinfo");
    std::cout << q.arguments.begin()->name << std::endl;
    //
    return 0;
}
