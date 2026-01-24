// ThorStormbreaker.cpp: define the entry point app.
#include "ThorStormbreaker.h"
#include "App.h"
#include <iostream>

using namespace std;

int main() {
    try {
        App app;
        app.start();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << std::endl;
    }
    return 0;
}
