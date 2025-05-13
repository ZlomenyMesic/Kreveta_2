//
// Created by michn on 5/12/2025.
//

#include <array>

#include "board.h"

#include <format>
#include <iostream>

#include "bitboard.h"
#include "uci.h"
#include "global/consts.h"
#include "movegen/move.h"

namespace Kreveta {

PieceType Board::piece_at(const uint8_t sq, const Color col) const {
    const uint64_t sq_mask = 1ULL << sq;

    for (int i = 0; i < 6; i++) {
        if (pieces[col][i] & sq_mask)
            return static_cast<PieceType>(i);
    }

    return PT_NONE;
}

void Board::play_move(const Move move) {

    // reset the en passant square and flip the side to move
    en_passant_sq = 64;
    color = col_flip(color);

    const uint8_t start_i = move.start();
    const uint8_t end_i   = move.end();

    const uint64_t start  = 1ULL << start_i;
    const uint64_t end    = 1ULL << end_i;

    const auto col = start & w_occupied
        ? COL_WHITE : COL_BLACK;

    const auto col_opp = col_flip(col);

    const PieceType piece = move.piece();
    const PieceType capt  = move.capture();
    const PieceType prom  = move.promotion();

    // en passant
    if (prom == PT_PAWN) {
        const uint64_t capt_sq = col == COL_WHITE
            ? end << 8
            : end >> 8;

        pieces[col_opp][PT_PAWN] ^= capt_sq;
        pieces[col    ][PT_PAWN] ^= start | end;

        if (col == COL_WHITE) {
            w_occupied ^= start | end;
            b_occupied ^= capt_sq;
        } else {
            b_occupied ^= start | end;
            w_occupied ^= capt_sq;
        }
    }

    // castling
    else if (prom == PT_KING) {
        const uint64_t rook = [&] {
            switch (end) {
                case 0x0000000000000004: return 0x0000000000000009ULL; // q
                case 0x0000000000000040: return 0x00000000000000A0ULL; // k
                case 0x0400000000000000: return 0x0900000000000000ULL; // Q
                case 0x4000000000000000: return 0xA000000000000000ULL; // K
                default: return 0ULL;
            }
        }();

        pieces[col][PT_KING] ^= start | end;
        pieces[col][PT_ROOK] ^= rook;

        if (col == COL_WHITE) w_occupied ^= rook | start | end;
        else                  b_occupied ^= rook | start | end;
    }

    // regular promotions (no overriding)
    else if (prom != PT_NONE) {
        pieces[col][piece] ^= start;
        pieces[col][prom]  ^= end;

        if (col == COL_WHITE) w_occupied ^= start | end;
        else                  b_occupied ^= start | end;
    }

    // regular move
    else {
        pieces[col][piece] ^= start | end;

        // if we double pushed a pawn, set the en passant square
        if (piece == PT_PAWN && (col == COL_WHITE
            ? start >> 16 == end
            : start << 16 == end)) {

            // en passant square is the square over which the
            // pawn has double pushed, not the capture square
            en_passant_sq = ls1b(col == COL_WHITE
                ? start >> 8
                : start << 8);
        }

        if (col == COL_WHITE) w_occupied ^= start | end;
        else                  b_occupied ^= start | end;
    }

    // captures
    if (capt != PT_NONE) {
        pieces[col_opp][capt] ^= end;

        if (col == COL_WHITE) b_occupied ^= end;
        else                  w_occupied ^= end;
    }

    // we moved the king => remove castling rights
    if (castling_rights && piece == PT_KING) {
        if (col == COL_WHITE) {
            remove_castling_right(CR_W_KINGSIDE);
            remove_castling_right(CR_W_QUEENSIDE);
        } else {
            remove_castling_right(CR_B_KINGSIDE);
            remove_castling_right(CR_B_QUEENSIDE);
        }
    }

    // a rook moved or was captured => remove castling right
    if (castling_rights && (piece == PT_ROOK || capt == PT_ROOK)) {

        // if rook moved we need the starting square
        // if rook was captured we need the ending square
        const auto rook_sq = piece == PT_ROOK
            ? start_i
            : end_i;

        switch (rook_sq) {
            case 0:  remove_castling_right(CR_B_QUEENSIDE); break;
            case 7:  remove_castling_right(CR_B_KINGSIDE);  break;
            case 56: remove_castling_right(CR_W_QUEENSIDE); break;
            case 63: remove_castling_right(CR_W_KINGSIDE);  break;
            default: break;
        }
    }
}

void Board::play_reversible_move(Move move, Color color) {

}

bool Board::is_move_legal(Move move, Color color) {
    return false;
}

void Board::print() const {
    char chars[64];
    std::fill_n(chars, 64, '-');

    for (int i = 0; i < 6; i++) {
        uint64_t w_copy = this->pieces[COL_WHITE][i];
        uint64_t b_copy = this->pieces[COL_BLACK][i];

        while (w_copy) {
            const uint8_t sq = ls1b_reset(w_copy);
            chars[sq] = std::toupper(PIECES.at(i));
        }

        while (b_copy) {
            const uint8_t sq = ls1b_reset(b_copy);
            chars[sq] = PIECES.at(i);
        }
    }

    for (int i = 0; i < 64; i++) {
        std::cout << chars[i] << ' ';

        // if we are at the end of a rank, we print a newline
        if ((i + 1 & 7) == 0) {
            std::cout << std::endl;
        }
    }
}

}
