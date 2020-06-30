#include "../board.hpp"

#include "rollouts.hpp"

namespace mcts {

float random_rollouts(const Board &board, const int number_of_rollouts) {
    float score = 0;

    for (int i = 0; i < number_of_rollouts; i++)
        score += random_rollout(board);

    return score / number_of_rollouts;
}

float random_rollout(Board board) {
    const int first_mover = board.turn;
    float state = board.state(false);
   
    while (state == UNFINISHED) {
        const std::vector<Move> &moves = board.genMoves();

        const Move &move = moves[rand() % moves.size()];

        board.make(move);
        state = board.state(false);

        if (board.fiftyMoves >= 100)
            state = DRAWN;
    }

    if (first_mover == board.turn)
        state = 1 - state;

    assert(state == LOST || state == DRAWN || state == WON);

    return state;
}

}