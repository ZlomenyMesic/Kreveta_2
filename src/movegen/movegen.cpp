//
// Created by michn on 5/13/2025.
//

#include "movegen.h"

#include "movetables.h"
#include "src/bitboard.h"

namespace Kreveta {

Move pseudo_legal_buffer[PL_BUFFER_SIZE];
int  cur_pl;

int Movegen::get_legal_moves(Board &board, Move *moves, bool only_captures) {
    cur_pl = 0;
    generate_pseudo_legal_moves(board, board.color);

    int legal_count = 0;
    for (int i = 0; i < cur_pl; i++) {
        *(moves + legal_count++) = pseudo_legal_buffer[i];
    }

    return legal_count;
}

void Movegen::generate_pseudo_legal_moves(const Board &board, const Color color) {

    // all occupied squares and squares occupied by opponent
    const uint64_t occupied = board.occupied();
    const uint64_t occupied_opp = color == COL_WHITE
        ? board.b_occupied
        : board.w_occupied;

    // all empty squares
    const uint64_t empty = ~occupied;
    // squares, where moves can end - empty or occupied by opponent (captures)
    const uint64_t free = empty | occupied_opp;

    // loop through every piece type and add start the respective move search loop
    for (int i = 0; i < 6; i++) {
        loop_pieces_bb(board,
            static_cast<PieceType>(i), color,
            board.pieces[color][i],
            occupied_opp, occupied, empty, free);
    }

    // castling when in check is illegal
    //if (IsKingInCheck(board, col))
    //    return;

    //ulong cast = King.GetCastlingTargets(board, col);
    //LoopTargets(board, BB.LS1B(board.Pieces[(byte)col][(byte)PType.KING]), cast, PType.NONE, col);
}

void Movegen::loop_pieces_bb(
    const Board    &board,   // the position for context
    const PieceType type,    // type of the moving piece
    const Color     color,   // color of the moving piece
          uint64_t  pieces,  // bitboard copy of the pieces
    const uint64_t  occ_opp, // all squares occupied by enemy
    const uint64_t  occ,     // all occupied squares
    const uint64_t  empty,   // all empty squares
    const uint64_t  free,    // empty or occupied by enemy squares
    const bool      only_captures) {

    const Color col_opp = col_flip(color);

    while (pieces) {
        // iteratively remove the pieces from the bitboard and generate their moves
        const int start = ls1b_reset(pieces);
        const uint64_t sq = 1ULL << start;

        // generate the moves
        uint64_t targets = get_targets(board, type, color, sq, occ_opp, occ, empty, free, only_captures);

        // loop the found moves and add them
        while (targets) {
            const int end = ls1b_reset(targets);
            PieceType capt = PT_NONE;

            // get the potential capture type
            if (type != PT_NONE) {
                for (int i = 0; i < 5; i++) {
                    if (!(board.pieces[col_opp][i] & 1ULL << end))
                        continue;

                    // we found the capture type
                    capt = static_cast<PieceType>(i);
                    break;
                }
            }

            // add the move
            add_move_to_buffer(type, color, capt, start, end, board.en_passant_sq);
        }
    }
}

uint64_t Movegen::get_targets(
    const Board    &board,
    const PieceType type,
    const Color     color,
    const uint64_t  sq,
    const uint64_t  occ_opp,
    const uint64_t  occ,
    const uint64_t  empty,
    const uint64_t  free,
    const bool      only_captures) {

    // return a bitboard of possible moves depending on the piece type
    switch (type) {
        case PT_PAWN: return (only_captures ? 0ULL
            : MoveTables::get_pawn_push_targets(sq, empty, color))
            | MoveTables::get_pawn_capt_targets(sq, occ_opp, board.en_passant_sq, color);

        case PT_KNIGHT: return MoveTables::get_knight_targets(sq, free);
        case PT_BISHOP: return MoveTables::get_bishop_targets(sq, free, occ);
        case PT_ROOK:   return MoveTables::get_rook_targets(sq, free, occ);

        case PT_QUEEN:  return MoveTables::get_bishop_targets(sq, free, occ)
                             | MoveTables::get_rook_targets(sq, free, occ);

        case PT_KING:   return MoveTables::get_king_targets(sq, free);
        default: return 0ULL;
    }
}

void Movegen::add_move_to_buffer(
    const PieceType type,
    const Color     color,
    const PieceType capt,
    const int       start,
    const int       end,
    const int       en_passant_sq) {

    // add the generated move to the list
    switch (type) {

        // pawns have a special designated method to prevent nesting (promotions)
        case PT_PAWN: {
            if ((end < 8  && color == COL_WHITE)
              | (end > 55 && color == COL_BLACK)) {

                // all four possible promotions
                pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_PAWN, capt, PT_KNIGHT);
                pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_PAWN, capt, PT_BISHOP);
                pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_PAWN, capt, PT_ROOK);
                pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_PAWN, capt, PT_QUEEN);
            }

            // en passant (pawn promotion)
            else if (end == en_passant_sq) {
                pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_PAWN, PT_NONE, PT_PAWN);
            }

            // regular pawn pushes and captures
            else pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_PAWN, capt, PT_NONE);
            return;
        }

        // special case for castling
        case PT_NONE: {
            pseudo_legal_buffer[cur_pl++] = Move(start, end, PT_KING, PT_NONE, PT_KING);
            return;
        }

        // any other move
        default: pseudo_legal_buffer[cur_pl++] = Move(start, end, type, capt, PT_NONE);
    }
}
}
