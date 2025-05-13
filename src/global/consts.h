//
// Created by michn on 5/11/2025.
//

#ifndef CONSTS_H
#define CONSTS_H

#include <string>
#include <cstdint>

namespace Kreveta {

constexpr std::string_view PIECES     = "pnbrqk";
constexpr std::string_view PROMOTIONS = "nbrq";
constexpr std::string_view FILES      = "abcdefgh";

constexpr std::string_view STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

extern const uint64_t REL_RANK_MASK[8];
extern const uint64_t REL_FILE_MASK[8];

extern const uint64_t A1H8_MASK[15];
extern const uint64_t A8H1_MASK[15];

extern const uint64_t FILE_MAGIC[8];
extern const uint64_t A1H8_MAGIC[15];
extern const uint64_t A8H1_MAGIC[15];

}

#endif //CONSTS_H
