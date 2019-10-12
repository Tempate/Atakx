#ifndef LOOKUP_H_
#define LOOKUP_H_

#include "bitboard.h"

void genLookupTables(void);

extern Bitboard singlesLookup[49];
extern Bitboard doublesLookup[49];

#endif // #ifndef LOOKUP_H_