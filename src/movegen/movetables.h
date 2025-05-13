//
// Created by michn on 5/13/2025.
//

#ifndef MOVETABLES_H
#define MOVETABLES_H

#include "src/global/types.h"

namespace Kreveta {

class MoveTables {
public:
    static void init();

    static uint64_t get_pawn_push_targets(uint64_t pawn, uint64_t empty, Color color);
    static uint64_t get_pawn_capt_targets(uint64_t pawn, uint64_t occ_opp, uint8_t en_passant_sq, Color color);

    static uint64_t get_king_targets(uint64_t king, uint64_t free);
    static uint64_t get_knight_targets(uint64_t knight, uint64_t free);

    static uint64_t get_bishop_targets(uint64_t bishop, uint64_t free, uint64_t occupied);
    static uint64_t get_rook_targets(uint64_t rook, uint64_t free, uint64_t occupied);

private:
    static void init_king_moves();
    static void init_knight_moves();

    static void init_rank_moves();
    static void init_file_moves();
    static void init_A1H8_moves();
    static void init_A8H1_moves();
};

}

#endif //MOVETABLES_H
