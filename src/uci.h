//
// Created by michn on 5/10/2025.
//

#ifndef UCI_H
#define UCI_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Kreveta {

class UCI {
public:

    static constexpr std::string_view ENGINE_NAME    = "Kreveta";
    static constexpr std::string_view ENGINE_AUTHOR  = "ZlomenyMesic";
    static constexpr int              ENGINE_VERSION = 2;

    template<typename T>
    static void log(const T &msg);
    static void log(const char *msg);

    template<typename ... Args>
    static void log_stats(const std::string &name, uint64_t value, Args... data);

    static void loop();

private:

    // enum to allow switch statements in command handling
    enum class Command : uint8_t {
        CMD_UCI, CMD_ISREADY, CMD_UCINEWGAME, CMD_STOP, CMD_QUIT, CMD_POSITION, CMD_GO, CMD_PERFT, CMD_D, CMD_TEST
    };

    static const std::unordered_map<std::string_view, Command> cmd_map;

    // just to handle the empty case
    static void log_stats_rec(){}

    template<typename ... Args>
    static void log_stats_rec(const std::string &name, uint64_t value, Args... data);

    static void handle_command(const std::string &command);

    inline static void cmd_uci() noexcept;
    inline static void cmd_isready() noexcept;
    inline static void cmd_d();

    inline static void cmd_position(const std::vector<std::string_view> &tokens);

#ifdef DEBUG
    static void cmd_test();
#endif

};

}

#endif //UCI_H
