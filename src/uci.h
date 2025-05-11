//
// Created by michn on 5/10/2025.
//

#ifndef UCI_H
#define UCI_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Kreveta {

class UCI {
public:

    static constexpr std::string_view ENGINE_NAME    = "Kreveta";
    static constexpr std::string_view ENGINE_AUTHOR  = "ZlomenyMesic";
    static constexpr int              ENGINE_VERSION = 2;

    static void log(const std::string_view &msg);
    static void loop();

private:

    // enum to allow switch statements in command handling
    enum class Command : uint8_t {
        CMD_UCI,
        CMD_ISREADY,
        CMD_UCINEWGAME,
        CMD_STOP,
        CMD_QUIT,
        CMD_POSITION,
        CMD_GO,
        CMD_PERFT,
        CMD_TEST
    };

    static const std::unordered_map<std::string_view, Command> cmd_map;

    static void handle_command(const std::string &command);

    inline static void cmd_uci();
    inline static void cmd_isready();

#ifdef DEBUG
    static void cmd_test();
#endif

};

}

#endif //UCI_H
