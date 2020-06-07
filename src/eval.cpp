#include "main.h"
#include "lookup.h"
#include "board.h"
#include "eval.h"

int countHoles(const Board &board, const int side);

const int tileValue = 100;

int eval(const Board &board) {
    const int material = board.pieces[board.turn].popCount() - board.pieces[board.turn ^ 1].popCount();
    const int holes = countHoles(board, board.turn) - countHoles(board, board.turn ^ 1);

    return material * tileValue + holes;
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