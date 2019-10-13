#ifndef MAIN_H_
#define MAIN_H_

#define ENGINE_NAME "Funes"
#define ENGINE_AUTHOR "tempate"

#include <array>
#include <cassert>
#include <iostream>
#include <vector>

#define DEBUG true

enum { RANDOM_PLAYER, MOST_CAPTURES, ALPHABETA, MCTS };

#define TYPE ALPHABETA

#define FILES 7
#define RANKS 7

#define MATE_SCORE 10000

#define N_SYM 4

#define FANCY_TT false

enum { BLUE, RED };

struct Stats {
    int ttHits;

    Stats() : ttHits(0) {}
};

#endif // #ifndef MAIN_H_