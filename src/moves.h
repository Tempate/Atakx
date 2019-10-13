#ifndef MOVES_H_
#define MOVES_H_

#include <string>

#include "main.h"
#include "bitboard.h"

enum { NULL_MOVE, SINGLE, DOUBLE };

struct Move {
    int from;
    int to;

    int type;
    int score;

    Move() : type(NULL_MOVE) {}

    Move(int to) : to(to), type(SINGLE) {}

    Move(int from, int to, int type) : from(from), to(to), type(type) {}

    Move(std::string moveString);

    std::string toString() const;

    void print() const { std::cout << toString() << std::endl; }

    constexpr bool operator==(const Move &move) const {
        if (type == SINGLE)
            return to == move.to;
        else
            return from == move.from && to == move.to;
    }

    int coordToSqr(std::string coord) {
        return Bitboard{}.getSquare(coord[0] - 'a', coord[1] - '1');
    }
};

#endif // #ifndef MOVES_H_