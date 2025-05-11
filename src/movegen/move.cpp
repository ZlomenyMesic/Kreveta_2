//
// Created by michn on 5/11/2025.
//

#include <string>

#include "move.h"

#include "../global/consts.h"

namespace Kreveta {

bool Move::is_valid_format(const std::string_view &move) {
    const std::size_t len = move.length();

    // any regular move will have 4 characters ("e2e4", "b8c6"),
    // which represent the starting and ending squares of the
    // move. an additional 5th character may be present in case
    // of promotion, where it denotes the promotion piece type
    if (len != 4 && len != 5)
        return false;

    // the ranks and files must be valid - files must
    // be a letter from 'a' to 'h', while ranks must be
    // a digit from 1 to 8
    if (move[0] < 'a' || move[0] > 'h'
     || move[1] < '1' || move[1] > '8'
     || move[2] < 'a' || move[2] > 'h'
     || move[3] < '1' || move[3] > '8')
        return false;

    if (len == 5) {
        // if we are promoting, we must check whether
        // we are promoting to a valid piece type
        if (PROMOTIONS.find(move[4]) == std::string::npos)
            return false;
    }

    return true;
}

// the move in the string is stored using a form of Long Algebraic Notation (LAN),
// which is used by UCI. there is no information about the piece moved; only the
// initial square and the square we landed on (e.g. "e2e4"). there may be an
// additional character for promotion (e.g. "e7e8q").
Move Move::str_to_move(const std::string_view &move) {

    // indices of starting and ending squares. we have already made
    // sure the string is correct, so there is no need to do it again
    const uint8_t start = static_cast<uint8_t>((8 - (move[1] - '0')) * 8) + FILES.find(move[0]);
    const uint8_t end   = static_cast<uint8_t>((8 - (move[3] - '0')) * 8) + FILES.find(move[2]);

    // the piece that moved and a potential capture
    const PieceType piece {PT_NONE};//UCI::board.piece_at(start).type;
    const PieceType capt  {PT_NONE};//UCI::board.piece_at(end).type;

    // regular promotions
    PieceType prom = move.length() == 5
        ? static_cast<PieceType>(PIECES.find(move[4]))
        : PT_NONE;

    // in case of castling, we override the promotion with PT_KING
    if (piece == PT_KING && (move == "e1g1" || move == "e1c1" || move == "e8g8" || move == "e8c8"))
        prom = PT_KING;

    // and in case of en passant, we override it with PT_PAWN. since en passant
    // doesn't actually land on the captured pawn, capture will be PT_NONE. then
    // we also check whether we changed files during the move
    if (piece == PT_PAWN && capt == PT_NONE && move[0] != move[2])
        prom = PT_PAWN;

    return {Move(start, end, piece, capt, prom)};
}

std::string_view Move::to_str(const Move move) {

    // maximum 5 characters + null terminator
    thread_local char buffer[6];
    int len {4};

    const uint8_t start = move.start();
    const uint8_t end   = move.end();
    const uint8_t prom  = move.promotion();

    // this lambda adds a square to the buffer with a specified offset
    const auto add_square = [&](const int index, const int offset) {
        buffer[offset]     = static_cast<char>('a' + (index & 7));
        buffer[offset + 1] = static_cast<char>('1' + (index >> 3));
    };

    // add the starting and ending squares
    add_square(start, 0);
    add_square(end,   2);

    // promotion is appended at the end, e.g. 'b7b8q'
    if (prom != PT_NONE && prom != PT_PAWN && prom != PT_KING) {
        buffer[4] = PIECES[prom];

        // we must now increase the length to copy the whole move
        len = 5;
    }

    return {std::string_view(buffer, len)};
}

}