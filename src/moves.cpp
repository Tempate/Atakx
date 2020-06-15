#include "lookup.hpp"
#include "moves.hpp"

Move::Move(std::string moveString) {
    if (moveString.length() == 2) {
        to = coordToSqr(moveString);
        type = SINGLE;
    } else {
        from = coordToSqr(moveString.substr(0, 2));
        to = coordToSqr(moveString.substr(2, 4));

        if (singlesLookup[from] & Bitboard{to})
            type = SINGLE;
        else
            type = DOUBLE;
    }
}

std::ostream& operator<<(std::ostream& os, const Move &move) {
    static const std::string coords[49] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", 
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", 
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", 
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", 
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", 
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", 
        "a7", "b7", "c7", "d7", "e7", "f7", "g7"
    };

    switch (move.type) {
    case SINGLE:
        os << coords[move.to];
        break;
    case DOUBLE:
        os << coords[move.from] + coords[move.to];
        break;
    case NULL_MOVE:
        os << "0000";
        break;
    }

    return os;
}

void Move::print() const {
    std::cout << *this << std::endl;
}