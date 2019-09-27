#ifndef BOARD_H_
#define BOARD_H_

#define FILES 7
#define RANKS 7

#include <cinttypes>
#include <string>

#include "main.h"
#include "moves.h"

class Board {
    public:
    uint64_t pieces[2];
    
    uint64_t empty;
    uint64_t occupied;
    
    uint64_t key;

    int turn;
    int opponent;

    Board();

    void startpos();
    void updateOccupancy();
    
    void fromFen(const std::string &fen);
    std::string toFen();

    std::vector<Move> genMoves();

    void make(const Move &move);

    void print();
};

constexpr static inline uint64_t getSqr(int file, int rank) { return (uint64_t) 1ULL << (7*rank+file); }
constexpr static inline int getRank(const int sqr) { return sqr / 7; }
constexpr static inline int getFile(const int sqr) { return sqr % 7; }

constexpr static inline uint64_t bitmask(const int i) { return (1LL << i); }

constexpr static inline int  popCount       (uint64_t bb)  { return __builtin_popcountll(bb); }
constexpr static inline int  bitScanForward (uint64_t bb)  { return __builtin_ctzll(bb); }
constexpr static inline int  bitScanReverse (uint64_t bb)  { return 63 - __builtin_clzll(bb); }

constexpr static inline uint64_t unsetLSB (uint64_t &bb) { return bb &= bb - 1; }
constexpr static inline uint64_t lsbBB    (uint64_t bb) { return bb & -bb; }

void printBB(const uint64_t bb);

#endif /* BOARD_H_ */