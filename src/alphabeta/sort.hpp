#ifndef SORT_HPP_
#define SORT_HPP_

#include "../board.hpp"

#define TT_MOVE std::numeric_limits<int>::max()

namespace alphabeta {
    void sort(const Board &board, std::vector<Move> &moves);
}

#endif // #ifndef SORT_HPP_
