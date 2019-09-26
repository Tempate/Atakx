#ifndef BOARD_H_
#define BOARD_H_

#define FILES 7
#define RANKS 7

#include <inttypes.h>
#include <string>

#define unsetLSB(bb) bb &= bb - 1
#define lsbBB(bb) bb & -bb

#define NO_CHECK 0xffffffffffffffff

typedef struct {
	uint64_t pieces[2];
    
	uint64_t empty;
	uint64_t occupied;
    
    uint64_t key;

	int turn;
	int opponent;
} Board;

#include "main.h"

static inline uint64_t getSqr(int file, int rank) { return (uint64_t) 1ULL << (7*rank+file); }
static inline int getRank(const int sqr) { return sqr / 7; }
static inline int getFile(const int sqr) { return sqr % 7; }

static inline uint64_t bitmask(const int i) { return (1LL << i); }

static inline int  popCount       (uint64_t bb)  { return __builtin_popcountll(bb); }
static inline int  bitScanForward (uint64_t bb)  { return __builtin_ctzll(bb); }
static inline int  bitScanReverse (uint64_t bb)  { return 63 - __builtin_clzll(bb); }

Board initialBoard(void);
Board blankBoard(void);

void updateOccupancy(Board &board);

Board fenToBoard(const std::string fen);
std::string boardToFen(const Board &board);

void printBoard(const Board &board);
void printBB(const uint64_t bb);

#endif /* BOARD_H_ */