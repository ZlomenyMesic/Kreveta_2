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
#include "movegen/movetables.h"

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

        // quit should exit the program immediately
        if (command == "quit")
            return;

        handle_command(command);
    }
}

void UCI::handle_command(const std::string &command) {
    const auto tokens = str_split(command);
    const auto cmd = tokens[0];

    if (cmd == "uci") {
        log(std::format("id name {}-{}\nid author {}", ENGINE_NAME, ENGINE_VERSION, ENGINE_AUTHOR));
        log("uciok");
    }

    else if (cmd == "isready") {
        log("readyok");
    }

    else if (cmd == "d") {
        Position::board.print();
    }

    else if (cmd == "position") {
        cmd_position(tokens);
    }

#ifdef DEBUG
    else if (cmd == "test") {
        cmd_test();
    }
#endif

    else {
        log(std::format("Unknown command '{}'", cmd));
        return;
    }
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
    log("Hello, World!");

    uint64_t king = 0x0800000000000000;

    log((int)popc(MoveTables::get_king_targets(king, 0xFFFFFFFFFFFFFFFF)));
}
#endif

}
