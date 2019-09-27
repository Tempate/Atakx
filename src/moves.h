#ifndef MOVES_H_
#define MOVES_H_

#include <vector>

enum {SINGLE, DOUBLE};

struct Move {
    int from;
    int to;
    int type;
    int color;

    Move(int from, int to, int type, int color):
        from(from), to(to), type(type), color(color) {}
};

#endif // #ifndef MOVES_H_