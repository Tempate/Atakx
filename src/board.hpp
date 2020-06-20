#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <chrono>
#include <cinttypes>
#include <string>

#include "main.hpp"
#include "bitboard.hpp"
#include "moves.hpp"

#define NOT_FINISHED 100

enum {MAKE, UNDO};

class Board {
public:
    Bitboard stones[2];
    Bitboard empty;
    Bitboard gaps;

    int turn;
    int ply;

    uint64_t key;

    int fiftyMoves = 0;

    Board();
    Board(const std::string &fen);

    void blank();
    void random();
    void startpos();

    void fromFen(const std::string &fen);
    std::string toFen() const;

    std::vector<Move> genMoves() const;
    int countMoves() const;

    std::vector<Move> genCaptures(const int sqr) const;
    bool isMoveLegal(const Move &move) const;
    int countCaptures(const Move &move) const;

    void make(const Move &move);
    void print() const;

    void playSequence(const std::string &moves);
    
    float state(const bool adjudicate) const;

    uint64_t perft(int depth) const;
    uint64_t ttPerft(int depth) const;
};

constexpr static inline int getRank(const int sqr) { return sqr / RANKS; }
constexpr static inline int getFile(const int sqr) { return sqr % FILES; }

#endif /* BOARD_HPP_ */