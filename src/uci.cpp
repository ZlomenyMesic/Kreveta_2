//
// Created by michn on 5/10/2025.
//

#include <string>
#include <iostream>
#include <format>

#include "uci.h"

#include "bitboard.h"
#include "position.h"
#include "utils.h"

namespace Kreveta {

// just to simplify syntax
template<typename T>
void UCI::log(const T &msg) {
    std::cout << msg << std::endl;
}

// the above function doesn't handle string literals, so we must overload it
void UCI::log(const char *msg) {
    std::cout << msg << std::endl;
}

template<typename ... Args>
void UCI::log_stats(const std::string &name, const uint64_t value, Args... data) {
    constexpr std::string_view STATS_HEADER = "---STATS-------------------------------";
    constexpr std::string_view STATS_AFTER  = "---------------------------------------";

    std::cout << STATS_HEADER << std::endl;
    log_stats_rec(name, value, data...);
    std::cout << STATS_AFTER << std::endl;
}

template<typename ... Args>
void UCI::log_stats_rec(const std::string &name, const uint64_t value, Args... data) {
    constexpr int DATA_OFFSET = 23;

    const int spaces = static_cast<int>(DATA_OFFSET - name.size());
    std::cout << name << ':' << (spaces
        ? std::string(spaces, ' ') : "") << format_uint64_t(value) << std::endl;

    log_stats_rec(data...);
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
        case Command::CMD_UCI:      cmd_uci();            break;
        case Command::CMD_ISREADY:  cmd_isready();        break;
        case Command::CMD_D:        cmd_d();              break;

        case Command::CMD_POSITION: cmd_position(tokens); break;

#ifdef DEBUG
        case Command::CMD_TEST:     cmd_test();           break;
#endif

        default: break;
    }
}

inline void UCI::cmd_uci() noexcept {
    log(std::format("id name {}-{}\nid author {}", ENGINE_NAME, ENGINE_VERSION, ENGINE_AUTHOR));
    log("uciok");
}

inline void UCI::cmd_isready() noexcept {
    log("readyok");
}

inline void UCI::cmd_d() {
    Position::board.print();
}


void UCI::cmd_position(const std::vector<std::string_view> &tokens) {
    if (tokens.size() < 2) {
        log("Missing arguments in position (startpos/fen).");
        return;
    }

    if (tokens[1] == "startpos") {
        Position::set_startpos(tokens);
    }

    else if (tokens[1] == "fen") {
        Position::set_position_fen(tokens);
    }

    else log(std::format("Invalid argument '{}'", tokens[1]));
}

#ifdef DEBUG
void UCI::cmd_test() {

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
    {"d",          Command::CMD_D},
    {"test",       Command::CMD_TEST},
};

}