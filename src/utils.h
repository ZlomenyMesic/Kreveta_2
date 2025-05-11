//
// Created by michn on 5/10/2025.
//

#ifndef UTILS_H
#define UTILS_H

#define STRING_UTILS

#include <string>
#include <vector>
#include <ranges>

namespace Kreveta {

#ifdef STRING_UTILS

__forceinline constexpr bool is_str_blank(const std::string_view &str) {
    return str.empty()
        || str.find_first_not_of(" \n\t\r") == std::string_view::npos;
}

inline std::vector<std::string_view> str_split(const std::string& str) {
    std::vector<std::string_view> tokens {};

    // ReSharper disable once CppLocalVariableMayBeConst
    auto split = str
        | std::ranges::views::split(' ')
        | std::ranges::views::transform([](auto range) {

        // we have to check the size of the range to prevent errors with
        // whitespaces. when there is a whitespace at the start or end or
        // a doubled one in the middle, we just return an empty string
        const auto dist = std::ranges::distance(range);

        // here we convert the range into a string view
        return dist
            ? std::string_view(&*range.begin(), dist)
            : std::string_view();
        });

    // since we may have added empty strings, we now have to look out for them
    for (const auto &token : split) {
        if (!is_str_blank(token)) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

template <typename T>
constexpr std::string uint_to_str(T value) {
    static_assert(std::is_unsigned_v<T>);

    // the maximum length of uint64_t is 20 digits, plus one for the
    // null terminator. this should be safe for all unsigned types
    char buffer[21];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

    // return empty string if conversion failed
    if (ec != std::errc()) {
        return {};
    }

    // create a string from the valid part of the buffer
    return std::string(buffer, ptr);
}

__forceinline constexpr std::string bool_to_str(const bool value) {
    return value ? "true" : "false";
}

#endif //STRING_UTILS

}


#endif //UTILS_H
