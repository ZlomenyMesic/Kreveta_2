//
// Created by michn on 5/11/2025.
//

#ifndef BITBOARD_H
#define BITBOARD_H

#include <bit>
#include <cstdint>

__forceinline constexpr uint8_t popc(const uint64_t bb) {
    return std::popcount<uint64_t>(bb);
}

__forceinline constexpr uint8_t ls1b(const uint64_t bb) {
    return std::countr_zero<uint64_t>(bb);
}

__forceinline constexpr uint8_t ls1b_reset(uint64_t &bb) {
    const auto index = std::countr_zero<uint64_t>(bb);
    bb &= bb - 1;
    return index;
}

__forceinline constexpr bool is_bit_set(const uint64_t bb, const uint8_t index) {
    return 1UL << index & bb;
}

#endif //BITBOARD_H
