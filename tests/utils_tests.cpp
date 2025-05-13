//
// Created by michn on 5/11/2025.
//

#include <catch2/catch_test_macros.hpp>

#include "src/utils.h"

TEST_CASE("is string blank") {
    constexpr std::string_view str1 = "";
    constexpr std::string_view str2 = "   \n\r";
    constexpr std::string_view str3 = "\n\r g?";

    REQUIRE(Kreveta::is_str_blank(str1) == true);
    REQUIRE(Kreveta::is_str_blank(str2) == true);
    REQUIRE(Kreveta::is_str_blank(str3) == false);
}

TEST_CASE("split string correctly") {
    const std::string str1 = "";
    const std::string str2 = "uci";
    const std::string str3 = "position   startpos moves    e2e4";

    REQUIRE(Kreveta::str_split(str1).size() == 0);
    REQUIRE(Kreveta::str_split(str2).size() == 1);
    REQUIRE(Kreveta::str_split(str3).size() == 4);

    REQUIRE(Kreveta::str_split(str3)[2] == "moves");
}
