#ifndef BOARD_H_
#define BOARD_H_

#include <chrono>
#include <cinttypes>
#include <string>

#include "bitboard.h"
#include "main.h"
#include "moves.h"

#define NOT_FINISHED 100

class Board {
public:
    Bitboard pieces[2];
    Bitboard empty;
    Bitboard gaps;

    int turn;
    int ply;

    uint64_t key;

    Board();
    Board(const std::string &fen);

    void blank();
    void random();
    void startpos();

    void fromFen(const std::string &fen);
    std::string toFen() const;

    std::vector<Move> genMoves() const;
    int countMoves() const;

    void make(const Move &move);

    void print() const;

    void playSequence(const std::string &moves);

    int eval() const;
    int score() const;
    float state() const;

    uint64_t perft(int depth) const;
    uint64_t ttPerft(int depth) const;

    void genKey(const bool symmetry);

    std::array<Board, N_SYM> genSymmetries();
    std::array<std::array<Bitboard, N_SYM>, 2> genBBSymmetries();

    std::chrono::high_resolution_clock::time_point
    timeManagement(std::chrono::high_resolution_clock::time_point start) const;
};

constexpr static inline int getRank(const int sqr) { return sqr / RANKS; }
constexpr static inline int getFile(const int sqr) { return sqr % FILES; }

#endif /* BOARD_H_ */