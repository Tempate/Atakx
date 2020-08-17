#include "../main.hpp"
#include "../board.hpp"
#include "../lookup.hpp"
#include "../hashtables.hpp"
#include "../eval.hpp"

#include "sort.hpp"

#define TT_MOVE std::numeric_limits<int>::max()

namespace alphabeta {

void sort(const Board &board, std::vector<Move> &moves) {
    static const std::array<int, 9> jumping_penalties = {0, 0, 0, 100, 200, 200, 300, 300, 400};

    const int empty = board.empty.popCount();
    const Entry &entry = tt.get_entry(board.key);

    for (Move &move : moves) {
        if (entry.key == board.key && move == entry.move) {
            move.score = TT_MOVE;
        } else {
            static const std::array<std::array<int, 9>, 2> score_by_captures {{
                {200, 300, 400, 600, 700, 800, 900, 1000, 1100},        // Single moves
                {0, 100, 200, 300, 400, 500, 600, 700, 800}             // Double moves
            }};

            move.captures = board.countCaptures(move);
            move.score = score_by_captures[move.type][move.captures];

            if (move.type == DOUBLE) {
                // Penalize jumps that leave a hole behind
                if ((neighboursLookup[move.from] & board.stones[board.turn ^ 1])) {
                    const int hole_size = (singlesLookup[move.from] & board.stones[board.turn]).popCount();
                    move.score -= jumping_penalties[hole_size];
                }
            }
        }
    }

    insertion_sort(moves);
}

}

void insertion_sort(std::vector<Move> &moves) {
    assert(moves.size() > 0);

    for (int i = 1; i < moves.size(); ++i) {
        const Move move = moves[i];
        int j = i - 1;

        while (j >= 0 && moves[j].score < move.score) {
            moves[j + 1] = moves[j];
            --j;
        }

        moves[j + 1] = move;
    }
}
