#include "../main.hpp"
#include "../board.hpp"
#include "../lookup.hpp"
#include "../hashtables.hpp"
#include "../sort.hpp"
#include "../eval.hpp"


namespace alphabeta {

void sort(const Board &board, std::vector<Move> &moves) {
    static const std::array<int, 9> jumping_penalties = {0, 0, 0, 100, 200, 200, 300, 300, 400};
    static const std::array<int, 9> hole_sealing_bonuses = {0, 0, 0, 100, 100, 200, 200, 300, 300};
    static const int SINGLE_MOVE_BONUS = 2;

    const Entry entry = tt.get_entry(board.key);

    for (Move &move : moves) {
        if (entry.key == board.key && move == entry.move) {
            move.score = std::numeric_limits<int>::max();
        } else {
            move.score = board.countCaptures(move) * stone_value;

            const int hole_size = (singlesLookup[move.from] & board.stones[board.turn]).popCount();

            switch (move.type) {
                case SINGLE:
                    move.score += SINGLE_MOVE_BONUS + hole_sealing_bonuses[hole_size];

                    break;
                case DOUBLE:
                    if (neighboursLookup[move.from] & board.stones[board.turn ^ 1])
                        move.score -= jumping_penalties[hole_size];

                    break;
            }
        }
    }

    insertion_sort(moves);
}

}

