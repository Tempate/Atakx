#include "board.hpp"


constexpr std::array<Bitboard, FILES * RANKS> gen_singles_lookup();
constexpr std::array<Bitboard, FILES * RANKS> gen_doubles_lookup();

constexpr std::array<Bitboard, FILES * RANKS> gen_neighbours_lookup();
constexpr std::array<Bitboard, FILES * RANKS> gen_surroundings_lookup();

constexpr std::array<Bitboard, FILES * RANKS> gen_liberties_lookup();
constexpr std::array<Bitboard, FILES * RANKS> gen_diagonals_lookup();


std::array<Bitboard, FILES * RANKS> singlesLookup = gen_singles_lookup();
std::array<Bitboard, FILES * RANKS> doublesLookup = gen_doubles_lookup();

std::array<Bitboard, FILES * RANKS> neighboursLookup = gen_neighbours_lookup();
std::array<Bitboard, FILES * RANKS> surroundingsLookup = gen_surroundings_lookup();

std::array<Bitboard, FILES * RANKS> libertiesLookup = gen_liberties_lookup();
std::array<Bitboard, FILES * RANKS> diagonalsLookup = gen_diagonals_lookup();


constexpr Bitboard notAFile   {(uint64_t) 0b1111110111111011111101111110111111011111101111110};
constexpr Bitboard notGFile   {(uint64_t) 0b0111111011111101111110111111011111101111110111111};

constexpr Bitboard notABFiles {(uint64_t) 0b1111100111110011111001111100111110011111001111100};
constexpr Bitboard notFGFiles {(uint64_t) 0b0011111001111100111110011111001111100111110011111};

constexpr Bitboard notABCFiles{(uint64_t) 0b1111000111100011110001111000111100011110001111000};
constexpr Bitboard notEFGFiles{(uint64_t) 0b0001111000111100011110001111000111100011110001111};


/* Singles
. . . . . . .         . . . . . . .
. . . . . . .         . . x x x . .
. . . x . . .         . . x . x . .
. . . . . . .    =>   . . x x x . .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
*/
constexpr std::array<Bitboard, FILES * RANKS> gen_singles_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table; 

    for (int i = 0; i < 49; ++i) {
        const Bitboard sqr(i);

        const Bitboard aux = sqr | (sqr << 7) | (sqr >> 7);
        const Bitboard east = (aux & notGFile) << 1;
        const Bitboard west = (aux & notAFile) >> 1;

        lookup_table[i] = (aux | east | west) ^ sqr & Bitboard{}.full();
    }

    return lookup_table;
}

/* Doubles
. . . . . . .         . x x x x x .
. . . . . . .         . x . . . x .
. . . x . . .         . x . x . x .
. . . . . . .    =>   . x . . . x .
. . . . . . .         . x x x x x .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
*/
constexpr std::array<Bitboard, FILES * RANKS> gen_doubles_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table;

    for (int i = 0; i < 49; ++i) {
        const Bitboard sqr(i);

        const Bitboard file = sqr | (sqr << 7) | (sqr << 14) | (sqr >> 14) | (sqr >> 7);
        const Bitboard east = ((file & notFGFiles) << 2) | ((file & notGFile) << 1);
        const Bitboard west = ((file & notABFiles) >> 2) | ((file & notAFile) >> 1);

        lookup_table[i] = (file | east | west) & ~(singlesLookup[i] | sqr) & Bitboard{}.full();
    }

    return lookup_table;
}

/* Neighbours
. . . . . . .         . x x x x x .
. . . . . . .         . x x x x x .
. . . x . . .         . x x . x x .
. . . . . . .    =>   . x x x x x .
. . . . . . .         . x x x x x .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
*/
constexpr std::array<Bitboard, FILES * RANKS> gen_neighbours_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table;

    for (int i = 0; i < 49; i++)
        lookup_table[i] = singlesLookup[i] | doublesLookup[i];

    return lookup_table;
}

/* Surroundings
. . . . . . .         x x x x x x x
. . . . . . .         x x x x x x x
. . . x . . .         x x x . x x x
. . . . . . .    =>   x x x x x x x
. . . . . . .         x x x x x x x
. . . . . . .         x x x x x x x
. . . . . . .         . . . . . . .
*/
constexpr std::array<Bitboard, FILES * RANKS> gen_surroundings_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table;

    for (int i = 0; i < 49; i++) {
        const Bitboard sqr(i);

        const Bitboard file = sqr | 
                              (sqr << 7) | (sqr << 14) | (sqr << 21) | 
                              (sqr >> 7) | (sqr >> 14) | (sqr >> 21);
        const Bitboard east = ((file & notEFGFiles) << 3) | ((file & notFGFiles) << 2) | ((file & notGFile) << 1);
        const Bitboard west = ((file & notABCFiles) >> 3) | ((file & notABFiles) >> 2) | ((file & notAFile) >> 1);

        lookup_table[i] = (file | east | west) ^ sqr & Bitboard{}.full();
    }

    return lookup_table;
}

/* Liberties
. . . . . . .         . . . . . . .
. . . . . . .         . . . x . . .
. . . x . . .         . . x . x . .
. . . . . . .    =>   . . . x . . .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
*/
constexpr std::array<Bitboard, FILES * RANKS> gen_liberties_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table; 

    for (int i = 0; i < 49; ++i) {
        const Bitboard sqr{i};

        const Bitboard file = (sqr << 7) | (sqr >> 7);
        const Bitboard east = ((sqr & notGFile) << 1);
        const Bitboard west = ((sqr & notAFile) >> 1);

        lookup_table[i] = (file | east | west) & Bitboard{}.full();
    }

    return lookup_table;
}

/* Diagonals
. . . . . . .         . . . . . . .
. . . . . . .         . . x . x . .
. . . x . . .         . . . . . . .
. . . . . . .    =>   . . x . x . .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
. . . . . . .         . . . . . . .
*/
constexpr std::array<Bitboard, FILES * RANKS> gen_diagonals_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table;

    for (int i = 0; i < 49; i++)
        lookup_table[i] = singlesLookup[i] ^ libertiesLookup[i];

    return lookup_table;
}