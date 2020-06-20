#include "main.hpp"
#include "lookup.hpp"
#include "board.hpp"
#include "eval.hpp"
#include "tuner/tuner.hpp"

#include <tuple>

int countHoles(const Board &board, const int side);
int psqtScore(const Board &board, const int side);
int holesScore(const Board &board, const int side);

const int stone_value = 100;

int eval(const Board &board) {
    const int material = board.stones[board.turn].popCount() - board.stones[board.turn ^ 1].popCount();
    
    #if TUNING
        const int psqt = tuner.tunePsqtScore(board, board.turn) - tuner.tunePsqtScore(board, board.turn ^ 1);
    #else
        const int psqt = psqtScore(board, board.turn) - psqtScore(board, board.turn ^ 1);
    #endif

    const int holes = holesScore(board, board.turn) - holesScore(board, board.turn ^ 1);

    return stone_value * (material + 10 * board.turn) + psqt + holes;
}

int psqtScore(const Board &board, const int side) {
    /*
    static const std::array<int, 49> psqt_normal = {
        90, 70, 70, 70, 70, 70, 90,
        70,  0,  0,  0,  0,  0, 70,
        70,  0,  0,  0,  0,  0, 70,
        70,  0,  0,  0,  0,  0, 70,
        70,  0,  0,  0,  0,  0, 70,
        70,  0,  0,  0,  0,  0, 70,
        90, 70, 70, 70, 70, 70, 90
    };

    static const std::array<int, 49> psqt_endgame = {
        45, 35, 35, 35, 35, 35, 45,
        35,  0,  0,  0,  0,  0, 35,
        35,  0,  0,  0,  0,  0, 35,
        35,  0,  0,  0,  0,  0, 35,
        35,  0,  0,  0,  0,  0, 35,
        35,  0,  0,  0,  0,  0, 35,
        45, 35, 35, 35, 35, 35, 45
    };

    const auto psqt = (board.empty.popCount() > 10) ? psqt_normal : psqt_endgame;
    */

    static const std::array<std::pair<Bitboard, int>, 3> psqt_normal = {
        std::make_pair(Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 90),
        std::make_pair(Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 70),
        std::make_pair(Bitboard{(uint64_t) 0b0000000000000000000000001000000000000000000000000}, 0)
    };

    static const std::array<std::pair<Bitboard, int>, 3> psqt_endgame = {
        std::make_pair(Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 30),
        std::make_pair(Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 10),
        std::make_pair(Bitboard{(uint64_t) 0b1100011110001100000000000000000000011000111100011}, 0)
    };

    const auto psqt = (board.empty.popCount() > 5) ? psqt_normal : psqt_endgame;

    int score = 0;

    for (const auto &[bb, value] : psqt)
        score += value * (bb & board.stones[side]).popCount();

    return score;
}

int holesScore(const Board &board, const int side) {
    static const std::array<int, 8> penalties = {0, 25, 50, 100, 150, 250, 350, 400};

    const Bitboard bb = board.stones[side].singles() & 
                        board.stones[side ^ 1].singles() &
                        board.empty;

    int score = 0;

    for (int sqr : bb) {
        const int weak = (singlesLookup[sqr] & board.stones[side]).popCount();
        score -= penalties[weak-1];
    }

    return score;
}
