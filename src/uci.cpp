//
// Created by michn on 5/10/2025.
//

#include <string>
#include <iostream>
#include <format>

#include "uci.h"

#include "utils.h"

namespace Kreveta {

// just to simplify syntax
void UCI::log(const std::string_view &msg) {
    std::cout << msg << std::endl;
}

void UCI::loop() {
    std::string command;
    while (std::getline(std::cin, command)) {

        if (is_str_blank(command))
            continue;

        handle_command(command);
    }
}

void UCI::handle_command(const std::string &command) {
    const auto tokens = str_split(command);
    const auto iterator = cmd_map.find(tokens.at(0));

    // the command is not present in the map => unknown command
    if (iterator == cmd_map.end()) {
        log(std::format("Unknown command '{}'", tokens.at(0)));
        return;
    }

    switch (iterator->second) {
        case Command::CMD_UCI:     cmd_uci();     break;
        case Command::CMD_ISREADY: cmd_isready(); break;

#ifdef DEBUG
        case Command::CMD_TEST:    cmd_test();    break;
#endif

        default:                                  break;
    }
}

inline void UCI::cmd_uci() {
    log(std::format("id name {}-{}\nid author {}", ENGINE_NAME, ENGINE_VERSION, ENGINE_AUTHOR));
    log("uciok");
}

inline void UCI::cmd_isready() {
    log("readyok");
}

#ifdef DEBUG
void UCI::cmd_test() {
    log("Hello, World!");
}
#endif

// maps all commands to their respective enum. while this is
// essentially useless, it allows us to process the command
// with a switch statement, which feels great
const std::unordered_map<std::string_view, UCI::Command> UCI::cmd_map = {
    {"uci",        Command::CMD_UCI},
    {"isready",    Command::CMD_ISREADY},
    {"ucinewgame", Command::CMD_UCINEWGAME},
    {"stop",       Command::CMD_STOP},
    {"quit",       Command::CMD_QUIT},
    {"position",   Command::CMD_POSITION},
    {"go",		  Command::CMD_GO},
    {"perft",      Command::CMD_PERFT},
    {"test",       Command::CMD_TEST},
};

}