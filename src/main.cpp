//
// Created by michn on 5/10/2025.
//

#include <format>
#include <iostream>

#include "uci.h"
#include "position.h"
#include "movegen/movetables.h"

int main(const int argc, [[maybe_unused]] char *argv[]) {
    using namespace Kreveta;

    // the first token is always the name
    if (argc > 1) {
        std::cout << "Command line arguments are not supported" << std::endl;
    }

    // initialize move lookup arrays
    MoveTables::init();

    // to avoid bugs, we have the startpos from the beginning
    Position::set_startpos({});

    // header text to be displayed
    UCI::log(std::format("{}-{} by {}", UCI::ENGINE_NAME, UCI::ENGINE_VERSION, UCI::ENGINE_AUTHOR));
    UCI::loop();

    return 0;
}