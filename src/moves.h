#ifndef MOVES_H_
#define MOVES_H_

#include <string>

#include "bitboard.h"
#include "main.h"

enum { NULL_MOVE, SINGLE, DOUBLE };

struct Move {
    int from;
    int to;

    int type;
    int score;

    Move() : type(NULL_MOVE) {}

    Move(int to) : to(to), type(SINGLE) {}

    Move(int from, int to, int type) : from(from), to(to), type(type) {}

    Move(std::string moveString) {
        if (moveString.length() == 2) {
            to = coordToSqr(moveString);
            type = SINGLE;
        } else {
            from = coordToSqr(moveString.substr(0, 2));
            to = coordToSqr(moveString.substr(2, 4));
            type = DOUBLE;
        }
    }

    std::string toString() const {
        static const std::string coords[49] = {
            "a1", "b1", "c1", "d1", "e1", "f1", "g1", "a2", "b2", "c2",
            "d2", "e2", "f2", "g2", "a3", "b3", "c3", "d3", "e3", "f3",
            "g3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "a5", "b5",
            "c5", "d5", "e5", "f5", "g5", "a6", "b6", "c6", "d6", "e6",
            "f6", "g6", "a7", "b7", "c7", "d7", "e7", "f7", "g7"};

        std::string move;

        switch (type) {
        case SINGLE:
            move = coords[to];
            break;
        case DOUBLE:
            move = coords[from] + coords[to];
            break;
        case NULL_MOVE:
            move = "0000";
            break;
        }

        return move;
    }

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