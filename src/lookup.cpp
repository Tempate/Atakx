
#include "board.h"

void genSingleMovesLookup(void);
void genDoubleMovesLookup(void);

uint64_t singlesLookup[49];
uint64_t doublesLookup[49];

const uint64_t notGFile = 0xfdfbf7efdfbf;
const uint64_t notAFile = 0x1fbf7efdfbf7e;

void genLookupTables(void) {
    genSingleMovesLookup();
    genDoubleMovesLookup();

    for (int i = 0; i < 49; ++i)
        printBB(doublesLookup[i]);
}

void genSingleMovesLookup(void) {

    for (int i = 0; i < 49; ++i) {
        const uint64_t sqr = bitmask(i);

        const uint64_t aux = sqr | (sqr << 7) | (sqr >> 7);
        const uint64_t east = (aux & notGFile) << 1;
        const uint64_t west = (aux & notAFile) >> 1;

        singlesLookup[i] = (aux | east | west) ^ sqr;
    }
}

void genDoubleMovesLookup(void) {

    static const uint64_t notFGFiles = 0x7cf9f3e7cf9f;
    static const uint64_t notABFiles = 0x1f3e7cf9f3e7c;

    for (int i = 0; i < 49; ++i) {
        const uint64_t sqr = bitmask(i);

        const uint64_t file = sqr | (sqr << 7) | (sqr << 14) | (sqr >> 14) | (sqr >> 7);
        const uint64_t east = ((file & notFGFiles) << 2) | ((file & notGFile) << 1);
        const uint64_t west = ((file & notABFiles) >> 2) | ((file & notAFile) >> 1);

        doublesLookup[i] = (file | east | west) & ~(singlesLookup[i] | sqr);
    }
}