#include "main.h"
#include "lookup.h"
#include "board.h"
#include "eval.h"
#include "tuner/tuner.h"

#include <tuple>

int countHoles(const Board &board, const int side);
int psqtScore(const Board &board, const int side);

const int tileValue = 100;

int eval(const Board &board) {
    const int material = board.pieces[board.turn].popCount() - board.pieces[board.turn ^ 1].popCount();
    const int holes = countHoles(board, board.turn) - countHoles(board, board.turn ^ 1);
    
    #if TUNING
        const int psqt = tuner.tunePsqtScore(board, board.turn) - tuner.tunePsqtScore(board, board.turn ^ 1);
    #else
        const int psqt = psqtScore(board, board.turn) - psqtScore(board, board.turn ^ 1);
    #endif

    return material * tileValue + holes + psqt;
}

int countHoles(const Board &board, const int side) {
    int holes = 0;

    for (int sqr : board.empty) {
        const Bitboard singles = singlesLookup[sqr];
        const Bitboard doubles = doublesLookup[sqr];
        const Bitboard neighborhood = singles | doubles;

        if (singles & board.pieces[side] && 
            !(neighborhood & board.pieces[side ^ 1]))
            holes++;
    }

    return holes;
}

int psqtScore(const Board &board, const int side) {
    static const std::array<int, FILES * RANKS> psqt = {
        120, 50, 59, 50, 50, 72, 120,
        44, 0, 0, -9, 10, 7, 44,
        48, 5, 14, 0, -2, -9, 60,
        55, 0, 7, -31, 0, -8, 53,
        49, -4, 0, 4, 0, 0, 62,
        99, 3, 0, -8, 3, -4, 46,
        119, 30, 42, 50, 60, 60, 120
    };

    Bitboard bb{(uint64_t) 1};
    int score = 0;

    for (const int value : psqt) {
        if (board.pieces[side] & bb)
            score += value;

        bb = bb << 1;
    }

    return score;
}