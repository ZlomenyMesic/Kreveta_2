//
// Created by michn on 5/13/2025.
//

#include <cstdint>

#include "movetables.h"

#include "src/bitboard.h"
#include "../global/consts.h"

namespace Kreveta {

static uint64_t king_moves[64];
static uint64_t knight_moves[64];

static uint64_t rank_moves[64][64];
static uint64_t file_moves[64][64];
static uint64_t A1H8_moves[64][64];
static uint64_t A8H1_moves[64][64];

void MoveTables::init() {
    init_king_moves();
    init_knight_moves();

    // rank moves must be initialized before other
    // sliders, since they're all based on them
    init_rank_moves();
    init_file_moves();
    init_A1H8_moves();
    init_A8H1_moves();
}

uint64_t MoveTables::get_pawn_push_targets(const uint64_t pawn, const uint64_t empty, const Color color) {

    // chess speaks for itself
    const uint64_t single_push = color == COL_WHITE
        ? pawn >> 8 & empty
        : pawn << 8 & empty;

    // another single push but make sure the pawn moved from the starting position
    const uint64_t double_push = color == COL_WHITE
        ? single_push >> 8 & empty & 0x000000FF00000000
        : single_push << 8 & empty & 0x00000000FF000000;

    return single_push | double_push;
}

uint64_t MoveTables::get_pawn_capt_targets(const uint64_t pawn, const uint64_t occ_opp, const uint8_t en_passant_sq, const Color color) {
    // in both cases we ensure the pawn hasn't jumped to the other side of the board

    // captures to the left
    const uint64_t left = color == COL_WHITE
        ? pawn >> 9 & 0x7F7F7F7F7F7F7F7F
        : pawn << 7 & 0x7F7F7F7F7F7F7F7F;

    // captures to the right
    const uint64_t right = color == COL_WHITE
        ? pawn >> 7 & 0xFEFEFEFEFEFEFEFE
        : pawn << 9 & 0xFEFEFEFEFEFEFEFE;

    const uint64_t en_passant_mask = en_passant_sq != 64
        ? 1ULL << en_passant_sq : 0ULL;

    // & with occupied sqaures of opposite color and en passant square
    return (left | right) & (occ_opp | en_passant_mask);
}

uint64_t MoveTables::get_king_targets(const uint64_t king, const uint64_t free) {
    // same as with knights, the king targets are indexed
    // directly by the square index and then & with empty
    // and enemy-occupied squares to avoid friendly captures
    const uint64_t targets = king_moves[ls1b(king)];
    return targets & free;
}

uint64_t MoveTables::get_knight_targets(const uint64_t knight, const uint64_t free) {
    // since knight moves aren't really based on pieces around (knights
    // can jump over them), we simply index the move bitboard directly
    // by the square index
    const uint64_t targets = knight_moves[ls1b(knight)];

    // we now just & the targets with both empty
    // and enemy squares to avoid friendly captures
    return targets & free;
}

uint64_t MoveTables::get_bishop_targets(const uint64_t bishop, const uint64_t free, const uint64_t occupied) {
    uint64_t targets = 0;
    const uint8_t sq = ls1b(bishop);

    int diag = 7 + (sq >> 3) - (sq & 7);
    int occupancy = static_cast<int>((occupied & A1H8_MASK[diag])
        * A1H8_MAGIC[diag] >> 57);

    targets |= A1H8_moves[sq][occupancy & 63];

    diag = (sq >> 3) + (sq & 7);
    occupancy = static_cast<int>((occupied & A8H1_MASK[diag])
        * A8H1_MAGIC[diag] >> 57);

    targets |= A8H1_moves[sq][occupancy & 63];

    return targets & free;
}

uint64_t MoveTables::get_rook_targets(const uint64_t rook, const uint64_t free, const uint64_t occupied) {
    // get the square index
    const uint8_t sq = ls1b(rook);

    // first only take the relevant bits
    int occupancy = static_cast<int>((occupied & REL_RANK_MASK[sq >> 3])
        // now we shift this bitboard by the rounded square
        // index (this essentially puts it on the first rank)
        >> ((sq >> 3) << 3));

    // now we take the lookup targets
    uint64_t targets = rank_moves[sq][occupancy >> 1 & 63];

    // this time we want the relevant file
    occupancy = static_cast<int>((occupied & REL_FILE_MASK[sq & 7])
        // and we multiply this by a magic number and then shift
        // it to once again allow correct lookup table indexing
        * FILE_MAGIC[sq & 7] >> 57);

    // and we add these file targets to the rank targets
    targets |= file_moves[sq][occupancy & 63];

    // we now have to & this result with free to avoid own captures
    // (occupied contains both friendly and enemy pieces, so at this
    // point we just & the targets with empty squares or enemy pieces)
    return targets & free;
}


void MoveTables::init_king_moves() {
    for (int sq = 0; sq < 64; sq++) {
        uint64_t king = 1ULL << sq;

        // starting, right and left square
        const uint64_t sides = king << 1 & 0xFEFEFEFEFEFEFEFE
                             | king >> 1 & 0x7F7F7F7F7F7F7F7F;
        king |= sides;

        // also move these up and down and remove the king from the center
        const uint64_t all = sides | king >> 8 | king << 8;
        king_moves[sq] = all;
    }
}

void MoveTables::init_knight_moves() {
    for (int sq = 0; sq < 64; sq++) {
        const uint64_t knight = 1ULL << sq;

        // right and left sqaures
        // again make sure we're not jumping across the whole board
        uint64_t right = knight << 1 & 0xFEFEFEFEFEFEFEFE;
        uint64_t left  = knight >> 1 & 0x7F7F7F7F7F7F7F7F;

        // shift the side squares up and down to generate "vertical" moves
        const uint64_t vertical = (right | left) >> 16
                                | (right | left) << 16;

        // shift the side squares to the side again
        right = right << 1 & 0xFEFEFEFEFEFEFEFE;
        left  = left  >> 1 & 0x7F7F7F7F7F7F7F7F;

        // move these up and down to generate "horizontal" moves
        const uint64_t horizontal = (right | left) >> 8
                                  | (right | left) << 8;

        knight_moves[sq] = vertical | horizontal;
    }
}

void MoveTables::init_rank_moves() {
    for (int sq = 0; sq < 64; sq++) {
        for (int o = 0; o < 64; o++) {

            const uint64_t occ     = o << 1;
            uint64_t targets = 0;

            // sliding to the right until we hit a blocker
            int slider = (sq & 7) + 1;
            while (slider <= 7) {
                targets |= 1ULL << slider;

                if (is_bit_set(occ, slider))
                    break;

                slider++;
            }

            // sliding to the left
            slider = (sq & 7) - 1;
            while (slider >= 0) {
                targets |= 1ULL << slider;

                if (is_bit_set(occ, slider))
                    break;

                slider--;
            }

            // move to correct rank
            targets <<= 8 * (sq >> 3);
            rank_moves[sq][o] = targets;
        }
    }
}

void MoveTables::init_file_moves() {
    for (int sq = 0; sq < 64; sq++) {
        for (int o = 0; o < 64; o++) {
            uint64_t targets = 0;
            const uint64_t rank_targets = rank_moves[7 - sq / 8][o];

            // rotate rank targets
            for (int bit = 0; bit < 8; bit++) {
                if (!is_bit_set(rank_targets, bit))
                    continue;

                targets |= 1ULL << ((sq & 7) + 8 * (7 - bit));
            }

            file_moves[sq][o] = targets;
        }
    }
}

void MoveTables::init_A1H8_moves() {
    for (int sq = 0; sq < 64; sq++) {
        for (int o = 0; o < 64; o++) {
            const int diag = (sq >> 3) - (sq & 7);

            uint64_t targets = 0;
            const uint64_t rank_targets = diag > 0
                ? rank_moves[sq & 7][o]
                : rank_moves[sq / 8][o];

            for (int bit = 0; bit < 8; bit++) {
                if (!is_bit_set(rank_targets, bit))
                    continue;

                int file, rank;

                if (diag >= 0) {
                    rank = diag + bit;
                    file = bit;
                }
                else {
                    file = bit - diag;
                    rank = bit;
                }

                if (file >= 0 && file <= 7
                 && rank >= 0 && rank <= 7) {
                    targets |= 1ULL << (file + 8 * rank);
                }
            }

            A1H8_moves[sq][o] = targets;
        }
    }
}

void MoveTables::init_A8H1_moves() {
    for (int sq = 0; sq < 64; sq++) {
        for (int o = 0; o < 64; o++) {
            const int diag = (sq >> 3) + (sq & 7);

            uint64_t targets = 0;
            const uint64_t rankTargets = diag > 7
                ? rank_moves[7 - sq / 8][o]
                : rank_moves[    sq & 7][o];

            for (int bit = 0; bit < 8; bit++) {

                // rotate rank moves
                if (!is_bit_set(rankTargets, bit))
                    continue;

                int rank, file;

                if (diag >= 7) {
                    rank = 7 - bit;
                    file = diag - 7 + bit;
                }
                else {
                    rank = diag - bit;
                    file = bit;
                }

                if (file >= 0 && file <= 7
                 && rank >= 0 && rank <= 7) {
                    targets |= 1ULL << (file + 8 * rank);
                }
            }

            A8H1_moves[sq][o] = targets;
        }
    }
}

}