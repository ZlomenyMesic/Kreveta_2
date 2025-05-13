//
// Created by michn on 5/13/2025.
//

#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "../board.h"

namespace Kreveta {

// pseudolegal moves are stored in a buffer to avoid repeated allocations
constexpr int PL_BUFFER_SIZE = 128;
extern Move pseudo_legal_buffer[PL_BUFFER_SIZE];
extern int  cur_pl;

class Movegen {
public:

    [[nodiscard]]
    static int get_legal_moves(Board &board, Move* moves, bool only_captures = false);

private:
    static void generate_pseudo_legal_moves(const Board &board, Color color);

    static void loop_pieces_bb(
        const Board &board,
        PieceType type,
        Color     color,
        uint64_t  pieces,
        uint64_t  occ_opp,
        uint64_t  occ,
        uint64_t  empty,
        uint64_t  free,
        bool      only_captures = false
    );

    static uint64_t get_targets(
        const Board &board,
        PieceType type,
        Color     color,
        uint64_t  sq,
        uint64_t  occ_opp,
        uint64_t  occ,
        uint64_t  empty,
        uint64_t  free,
        bool      only_captures
    );

    static void add_move_to_buffer(
        PieceType type,
        Color     color,
        PieceType capt,
        int       start,
        int       end,
        int       en_passant_sq
    );
};
}

#endif //MOVEGEN_H
