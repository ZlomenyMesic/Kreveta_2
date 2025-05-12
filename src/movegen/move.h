//
// Created by michn on 5/11/2025.
//

#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <string_view>

#include "../global/types.h"
#include "src/board.h"

namespace Kreveta {

class Board;

struct Move {
    constexpr Move(const uint8_t start, const uint8_t end, const PieceType piece, const PieceType capture, const PieceType promotion) {

        // flags are only set when the constructor is called, after that they cannot be changed.
        // there isn't really a reason to have an option to change them later
        _flags |= start;
        _flags |= end       << END_OFFSET;
        _flags |= piece     << PIECE_OFFSET;
        _flags |= capture   << CAPT_OFFSET;
        _flags |= promotion << PROM_OFFSET;
    }


    // sometimes we have to compare moves and it's very simple
    constexpr bool operator ==(const Move& m) const {
        return _flags == m._flags;
    }

    constexpr bool operator !=(const Move& m) const {
        return _flags != m._flags;
    }

    // when getting this information, we first & the flags and the
    // correct mask - this keeps only the required bits, and those
    // we then shift correctly to leave us with a useful number

    [[nodiscard]]
    __forceinline constexpr uint8_t start() const noexcept {
        return _flags & START_MASK;
    }

    [[nodiscard]]
    __forceinline constexpr uint8_t end() const noexcept {
        return (_flags & END_MASK) >> END_OFFSET;
    }

    __forceinline PieceType piece() const noexcept {
        return static_cast<PieceType>((_flags & PIECE_MASK) >> PIECE_OFFSET);
    }

    __forceinline PieceType capture() const noexcept {
        return static_cast<PieceType>((_flags & CAPT_MASK) >> CAPT_OFFSET);
    }

    __forceinline PieceType promotion() const noexcept {
        return static_cast<PieceType>((_flags & PROM_MASK) >> PROM_OFFSET);
    }

    static bool is_valid_format(const std::string_view &move);
    static Move str_to_move(const std::string_view &move, const Board &context);
    static std::string_view to_str(Move move);

private:
    /*
    although it's completely possible to encode a move into 16 bits, we are using 32.
    this is the format:

                          | prom. | capt. | piece | end         | start
    0 0 0 0 0 0 0 0 0 0 0 | 0 0 0 | 0 0 0 | 0 0 0 | 0 0 0 0 0 0 | 0 0 0 0 0 0
    */
    uint32_t _flags {0};

    static constexpr uint32_t END_OFFSET   = 6;
    static constexpr uint32_t PIECE_OFFSET = 12;
    static constexpr uint32_t CAPT_OFFSET  = 15;
    static constexpr uint32_t PROM_OFFSET  = 18;

    static constexpr uint32_t START_MASK = 0x0000003F;
    static constexpr uint32_t END_MASK   = 0x00000FC0;
    static constexpr uint32_t PIECE_MASK = 0x00007000;
    static constexpr uint32_t CAPT_MASK  = 0x00038000;
    static constexpr uint32_t PROM_MASK  = 0x001C0000;
};

}

#endif //MOVE_H
