#ifndef MOVES_H_
#define MOVES_H_

#include <vector>

#include "board.h"

enum {SINGLE, DOUBLE};

typedef struct {
    int from;
    int to;
    int type;
    int color;
} Move;

void makeMove(Board &board, const Move &move);

std::vector<Move> genMoves(Board &board);

#endif // #ifndef MOVES_H_