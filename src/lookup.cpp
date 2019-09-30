#include "board.h"

void genSingleMovesLookup(void);
void genDoubleMovesLookup(void);

Bitboard singlesLookup[49];
Bitboard doublesLookup[49];

const Bitboard notGFile((uint64_t) 0xfdfbf7efdfbf);
const Bitboard notAFile((uint64_t) 0x1fbf7efdfbf7e);

void genLookupTables(void) {
    genSingleMovesLookup();
    genDoubleMovesLookup();
}

void genSingleMovesLookup(void) {
    for (int i = 0; i < 49; ++i) {
        Bitboard sqr(i);

        Bitboard aux = sqr | (sqr << 7) | (sqr >> 7);
        Bitboard east = (aux & notGFile) << 1;
        Bitboard west = (aux & notAFile) >> 1;

        singlesLookup[i] = (aux | east | west) ^ sqr;
    }
}

void genDoubleMovesLookup(void) {
    static const Bitboard notFGFiles = Bitboard{(uint64_t) 0x7cf9f3e7cf9f};
    static const Bitboard notABFiles = Bitboard{(uint64_t) 0x1f3e7cf9f3e7c};

    for (int i = 0; i < 49; ++i) {
        Bitboard sqr(i);

        Bitboard file = sqr | (sqr << 7) | (sqr << 14) | (sqr >> 14) | (sqr >> 7);
        Bitboard east = ((file & notFGFiles) << 2) | ((file & notGFile) << 1);
        Bitboard west = ((file & notABFiles) >> 2) | ((file & notAFile) >> 1);

        doublesLookup[i] = (file | east | west) & ~(singlesLookup[i] | sqr);
    }
}