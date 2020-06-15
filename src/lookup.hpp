#ifndef LOOKUP_H_
#define LOOKUP_H_

#include "bitboard.hpp"

extern std::array<Bitboard, FILES * RANKS> singlesLookup;
extern std::array<Bitboard, FILES * RANKS> doublesLookup;

#endif // #ifndef LOOKUP_H_