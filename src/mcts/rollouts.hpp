#ifndef ROLLOUTS_HPP_
#define ROLLOUTS_HPP_

#include "../board.hpp"

namespace mcts {
    float random_rollouts(const Board &board, const int number_of_rollouts);
    float random_rollout(Board board);
}

#endif // #ifndef ROLLOUTS_HPP_
