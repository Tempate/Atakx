#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include <vector>
#include <cinttypes>

#include "main.h"
#include "moves.h"
#include "bitboard.h"

class Board {
    public:
        Bitboard pieces[2];
        Bitboard empty;
        Bitboard gaps;

        int turn;
        int opponent;

        uint64_t key;

        Board();
        Board(const std::string &fen);

        void blank();
        void startpos();
        
        void fromFen(const std::string &fen);
        std::string toFen() const;

        std::vector<Move> genMoves() const;

        void make(const Move &move);

        void print() const;

        void playSequence(const std::string &moves);

        int eval() const;
        int score() const;

        uint64_t perft(int depth) const;
};

constexpr static inline int getRank(const int sqr) { 
    return sqr / RANKS; 
}

constexpr static inline int getFile(const int sqr) { 
    return sqr % FILES; 
}

#endif /* BOARD_H_ */