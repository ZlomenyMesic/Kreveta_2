//
// Created by michn on 5/12/2025.
//

#include <format>

#include "position.h"

#include "uci.h"
#include "utils.h"
#include "board.h"
#include "global/consts.h"
#include "movegen/move.h"

namespace Kreveta {

Board Position::board;
Color Position::engine_color;

void Position::set_startpos(const std::vector<std::string_view> &tokens) {
    auto new_board = Board::make_startpos();

    if (try_play_moves(tokens, new_board)) {
        board        = new_board.clone();
        engine_color = new_board.color;
    }
}

void Position::set_position_fen(const std::vector<std::string_view> &tokens) {

    // if something is missing, we return immediately instead of wasting time
    if (const auto size = tokens.size(); size < 6) {
        UCI::log("Incomplete or invalid FEN");
        return;
    }

    // we don't want to modify Position::board right away in case something goes wrong
    Board new_board;

    // the first two tokens are always 'position fen', so when I refer to the "first"
    // token, I am in fact talking about the third token.

    // the first token is the actual position. all ranks are separated by a "/". between
    // the slashes, pieces may be denoted with the simple "pnbrqk" or the uppercase variants
    // for white. empty squares between pieces are marked by a single digit (1-8)
    for (int i = 0, sq = 0; i < tokens[2].size(); i++) {
        const char c = tokens[2][i];

        // increase the square counter (empty squares)
        if (std::isdigit(c)) {
            sq += c - '0';
            continue;
        }

        // the character is not a valid piece
        if (!PIECES.contains(static_cast<char>(std::tolower(c)))) {

            // check for a slash, which indicates another rank. although we don't need this
            // information, we must check for it to not return false invalid FEN warnings
            if (c == '/')
                continue;

            // if the character is neither a digit, piece nor slash, it's incorrect
            UCI::log(std::format("Invalid character in FEN '{}'", c));
            return;
        }

        // uppercase characters represent white color
        const Color col = std::isupper(c)
            ? COL_WHITE : COL_BLACK;

        // we have already ensured previously that the piece is present in PIECES
        const auto pt = static_cast<PieceType>(PIECES.find(static_cast<char>(std::tolower(c))));

        new_board.pieces[col][pt] |= 1ULL << sq;

        if (col == COL_WHITE) new_board.w_occupied |= 1ULL << sq;
        else                  new_board.b_occupied |= 1ULL << sq;

        sq++;
    }

    // the second token tells us, which color's turn it is. "w" means white,
    // "b" means black. the actual color to play may be still modified by the
    // moves played from this position, though
    switch (const char c = tokens[3][0]) {
        case 'w': new_board.color = COL_WHITE; break;
        case 'b': new_board.color = COL_BLACK; break;

        default: {
            UCI::log(std::format("Invalid color '{}'", c));
            return;
        };
    }

    // the third token marks, which sides still have their castling rights. if neither
    // side can castle, this is a dash. otherwise, the characters may be "k" or "q" for
    // kingside and queenside castling respectively, or once again uppercase for white.
    // just to clarify, this has nothing to do with the legality of castling in the
    // next move, this only denotes the castling rights availability.
    for (const char c : tokens[4]) {
        switch (c) {
            case 'K': new_board.add_castling_right(CR_W_KINGSIDE);  break;
            case 'Q': new_board.add_castling_right(CR_W_QUEENSIDE); break;
            case 'k': new_board.add_castling_right(CR_B_KINGSIDE);  break;
            case 'q': new_board.add_castling_right(CR_B_QUEENSIDE); break;

            case '-': break;
            default: {
                UCI::log(std::format("Invalid castling availability '{}'", c));
                return;
            }
        }
    }

    // the fourth token is the en passant square, which is the square over which
    // a double-pushing pawn has passed in the previous move, regardless of whether
    // there is another pawn to capture en passant. if no pawn double-pushed, this
    // is also simply a dash.
    if (int en_passant_sq; try_parse(tokens[5], en_passant_sq)) {
        new_board.en_passant_sq = static_cast<uint8_t>(en_passant_sq);
    }
    else if (tokens[5] != "-") {
        UCI::log(std::format("Invalid en passant square '{}'", tokens[5]));
        return;
    }

    // after these tokens may also follow a fullmove and halfmove clock,
    // but we don't need this information for anything

    // the fen string can be followed by a sequence of moves, which have
    // been played from the position. for example, most GUIs would pass
    // a position like "position startpos moves e2e4 e7e5 g1f3"
    if (!try_play_moves(tokens, new_board)) {
        return;
    }

    // only after we made sure that all arguments were passed
    // correctly, we change the actual board state. if we did
    // it directly, then the board state would become corrupt
    // after the user set an incorrect position
    board        = new_board;
    engine_color = new_board.color;
}

bool Position::try_play_moves(const std::vector<std::string_view> &tokens, Board &new_board) {
    const auto iterator = std::ranges::find(tokens, "moves");

    // if no moves follow up
    if (iterator == tokens.end()) {
        return true;
    }

    const auto moves_i = static_cast<int>(std::distance(tokens.begin(), iterator));
    for (int i = moves_i + 1; i < tokens.size(); i++) {

        if (!Move::is_valid_format(tokens[i])) {
            UCI::log(std::format("Invalid move '{}'", tokens[i]));
            return false;
        }

        new_board.play_move(Move::str_to_move(tokens[i], new_board));
    }

    return true;
}

}
