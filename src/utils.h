//
// Created by michn on 5/10/2025.
//

#ifndef UTILS_H
#define UTILS_H

#include <iomanip>
#include <string>
#include <vector>
#include <ranges>

namespace Kreveta {

// we need this to allow pretty numbers in stats
struct comma_numpunct final : std::numpunct<char> {
    char do_decimal_point() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};

// convert a number to string with pretty number separators
inline std::string format_uint64_t(const uint64_t n) {
    std::ostringstream os;
    os.imbue(std::locale(std::locale(), new comma_numpunct));
    os << n;
    return os.str();
}

// check whether a string is empty or consists of just whitespace characters
constexpr bool is_str_blank(const std::string_view &str) noexcept {
    return str.empty()
        || str.find_first_not_of(" \n\t\r") == std::string_view::npos;
}

// split a string into tokens using a space as the delimeter
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

inline bool try_parse(const std::string_view &str, int& out_value) {
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), out_value);
    return ec == std::errc();
}

}


#endif //UTILS_H
