#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include <vector>

#include "main.h"
#include "moves.h"
#include "bitboard.h"

class Board {
    public:
        Bitboard pieces[2];
        
        Bitboard empty;
        Bitboard occupied;
        
        Bitboard key;

        int turn;
        int opponent;

        void updateOccupancy();

        Board();

        void blank();
        void startpos();
        
        void fromFen(const std::string &fen);
        std::string toFen() const;

        std::vector<Move> genMoves() const;

        void make(const Move &move);

        void print() const;

        void playSequence(const std::string &moves);

        int eval() const;
};

constexpr static inline int getRank(const int sqr) { 
    return sqr / RANKS; 
}

constexpr static inline int getFile(const int sqr) { 
    return sqr % FILES; 
}

#endif /* BOARD_H_ */