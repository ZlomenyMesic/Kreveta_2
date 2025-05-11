//
// Created by michn on 5/11/2025.
//

#include <catch2/catch_test_macros.hpp>

#include "src/bitboard.h"

constexpr uint64_t a = 0ULL;
constexpr uint64_t b = 0xFFFFFFFFFFFFFFF0ULL;
constexpr uint64_t c = 0x8000000000000000ULL;

TEST_CASE("LS1B index") {
    REQUIRE(ls1b(a) == 64);
    REQUIRE(ls1b(b) == 4);
    REQUIRE(ls1b(c) == 63);
}

TEST_CASE("LS1B index with reset") {
    auto a_copy = a;
    auto b_copy = b;
    auto c_copy = c;

    REQUIRE(ls1b_reset(a_copy) == 64);
    REQUIRE(ls1b_reset(b_copy) == 4);
    REQUIRE(ls1b_reset(c_copy) == 63);

    REQUIRE(ls1b_reset(a_copy) == 64);
    REQUIRE(ls1b_reset(b_copy) == 5);
    REQUIRE(ls1b_reset(c_copy) == 64);
}

TEST_CASE("popcount") {
    REQUIRE(popc(a) == 0);
    REQUIRE(popc(b) == 60);
    REQUIRE(popc(c) == 1);
}

TEST_CASE("is bit set") {

    // this is a programming war crime. don't judge me
    REQUIRE(is_bit_set(a, 61) == false);
    REQUIRE(is_bit_set(b, 61) == true);
    REQUIRE(is_bit_set(c, 61) == false);
}