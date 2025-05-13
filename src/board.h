//
// Created by michn on 5/12/2025.
//

#ifndef BOARD_H
#define BOARD_H

#include <cstdint>

#include "global/types.h"
#include "movegen/move.h"

namespace Kreveta {

struct Move;

class Board {
public:

    uint64_t pieces[2][6]{};

    uint64_t w_occupied      = 0ULL;
    uint64_t b_occupied      = 0ULL;

    uint8_t  en_passant_sq   = 64;
    uint8_t  castling_rights = CR_ALL;
    Color    color           = COL_WHITE;

    // return a bitboard with all empty squares on the board
    [[nodiscard]] constexpr uint64_t empty() const {
        return ~(this->w_occupied | this->b_occupied);
    }

    // return a bitboard with all occupied squares on the board
    [[nodiscard]] constexpr uint64_t occupied() const {
        return this->w_occupied | this->b_occupied;
    }

    [[nodiscard]] PieceType piece_at(uint8_t sq, Color col) const;

    constexpr void add_castling_right(const CastlingRights cr) {
        castling_rights |= cr;
    }

    constexpr void remove_castling_right(const CastlingRights cr) {
        castling_rights &= ~cr;
    }

    [[nodiscard]] constexpr bool has_castling_right(const CastlingRights cr) const {
        return castling_rights & cr;
    }

    void play_move(Move move);
    void play_reversible_move(Move move, Color color);

    bool is_move_legal(Move move, Color color);

    void print() const;

    [[nodiscard]] Board clone() const {
        return *this;
    }

    constexpr static Board make_startpos() {
        Board board;

        board.pieces[COL_WHITE][PT_PAWN]   = 0x00FF000000000000ULL;
        board.pieces[COL_WHITE][PT_KNIGHT] = 0x4200000000000000ULL;
        board.pieces[COL_WHITE][PT_BISHOP] = 0x2400000000000000ULL;
        board.pieces[COL_WHITE][PT_ROOK]   = 0x8100000000000000ULL;
        board.pieces[COL_WHITE][PT_QUEEN]  = 0x0800000000000000ULL;
        board.pieces[COL_WHITE][PT_KING]   = 0x1000000000000000ULL;

        board.pieces[COL_BLACK][PT_PAWN]   = 0x000000000000FF00ULL;
        board.pieces[COL_BLACK][PT_KNIGHT] = 0x0000000000000042ULL;
        board.pieces[COL_BLACK][PT_BISHOP] = 0x0000000000000024ULL;
        board.pieces[COL_BLACK][PT_ROOK]   = 0x0000000000000081ULL;
        board.pieces[COL_BLACK][PT_QUEEN]  = 0x0000000000000008ULL;
        board.pieces[COL_BLACK][PT_KING]   = 0x0000000000000010ULL;

        board.w_occupied                   = 0xFFFF000000000000ULL;
        board.b_occupied                   = 0x000000000000FFFFULL;

        board.en_passant_sq                = 64;
        board.castling_rights              = CR_ALL;
        board.color                        = COL_WHITE;

        return board;
    }
};

}

#endif //BOARD_H
