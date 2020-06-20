#ifndef LOOKUP_H_
#define LOOKUP_H_

#include "bitboard.hpp"

extern std::array<Bitboard, FILES * RANKS> singlesLookup;
extern std::array<Bitboard, FILES * RANKS> doublesLookup;

extern std::array<Bitboard, FILES * RANKS> neighboursLookup;
extern std::array<Bitboard, FILES * RANKS> surroundingsLookup;

extern std::array<Bitboard, FILES * RANKS> libertiesLookup;
extern std::array<Bitboard, FILES * RANKS> diagonalsLookup;

#endif // #ifndef LOOKUP_H_