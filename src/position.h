//
// Created by michn on 5/12/2025.
//

#ifndef POSITION_H
#define POSITION_H

#include <vector>

#include "board.h"

namespace Kreveta {

class Position {
public:

	static Board board;
	static Color engine_color;

	static void set_startpos(const std::vector<std::string_view> &tokens);
	static void set_position_fen(const std::vector<std::string_view> &tokens);
	static bool try_play_moves(const std::vector<std::string_view> &tokens, Board &new_board);
};

}

#endif //POSITION_H
