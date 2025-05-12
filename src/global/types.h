//
// Created by michn on 5/11/2025.
//

#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace Kreveta {

// all enums are defined as enums, not enum classes. this,
// however, has a very logical reason, since we want to
// allow really simple indexing, like [COL_WHITE][PT_PAWN],
// even at the cost of polluting this namespace

// Color and PieceType NONE values cannot be zero, because
// these values are also used to index the piece bitboards

enum Color : uint8_t {
    COL_WHITE = 0,
    COL_BLACK = 1,

    COL_NONE  = 2
};

__forceinline constexpr Color col_flip(const Color col) {
    return col == COL_WHITE ? COL_BLACK : COL_WHITE;
}

enum PieceType : uint8_t {
    PT_PAWN   = 0,
    PT_KNIGHT = 1,
    PT_BISHOP = 2,
    PT_ROOK   = 3,
    PT_QUEEN  = 4,
    PT_KING   = 5,

    PT_NONE   = 6
};

// these shall be used as flags - we can combine
// them or do some other magic with bit operations
enum CastlingRights : uint8_t {
    CR_NONE = 0,

    CR_W_KINGSIDE  = 1,
    CR_W_QUEENSIDE = 2,

    CR_B_KINGSIDE  = 4,
    CR_B_QUEENSIDE = 8,

    CR_ALL = CR_W_KINGSIDE | CR_B_KINGSIDE | CR_W_QUEENSIDE | CR_B_QUEENSIDE
};

}

#endif //TYPES_H
