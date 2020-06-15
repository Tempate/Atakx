#ifndef UAI_HPP_
#define UAI_HPP_

#include <cinttypes>

#include "moves.hpp"

#define DEF_DEPTH 5
#define MAX_DEPTH 50

struct Settings {
    bool stop;

    int depth;
    uint64_t nodes;

    int wtime;
    int btime;
    int winc;
    int binc;

    int movetime;
    bool timed;

    void init() {
        stop = false;

        depth = MAX_DEPTH;
        nodes = 0;

        wtime = 0;
        btime = 0;
        winc = 0;
        binc = 0;

        movetime = 0;

        timed = true;
    }
};

void uai();

#endif // #ifndef UAI_HPP_