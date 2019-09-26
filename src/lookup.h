#ifndef LOOKUP_H_
#define LOOKUP_H_

#include <inttypes.h>

void genLookupTables(void);

extern uint64_t singlesLookup[49];
extern uint64_t doublesLookup[49];

#endif // #ifndef LOOKUP_H_