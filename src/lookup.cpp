#include "board.hpp"

constexpr std::array<Bitboard, FILES * RANKS> gen_single_moves_lookup();
constexpr std::array<Bitboard, FILES * RANKS> gen_double_moves_lookup();

constexpr Bitboard notGFile{(uint64_t) 0x0fdfbf7efdfbf};
constexpr Bitboard notAFile{(uint64_t) 0x1fbf7efdfbf7e};

std::array<Bitboard, FILES * RANKS> singlesLookup = gen_single_moves_lookup();
std::array<Bitboard, FILES * RANKS> doublesLookup = gen_double_moves_lookup();


constexpr std::array<Bitboard, FILES * RANKS> gen_single_moves_lookup() {
    std::array<Bitboard, FILES * RANKS> lookup_table = {0}; 

    for (int i = 0; i < 49; ++i) {
        const Bitboard sqr(i);

        const Bitboard aux = sqr | (sqr << 7) | (sqr >> 7);
        const Bitboard east = (aux & notGFile) << 1;
        const Bitboard west = (aux & notAFile) >> 1;

        lookup_table[i] = (aux | east | west) ^ sqr;
    }

    return lookup_table;
}

constexpr std::array<Bitboard, FILES * RANKS> gen_double_moves_lookup() {
    constexpr Bitboard notFGFiles = Bitboard{(uint64_t) 0x07cf9f3e7cf9f};
    constexpr Bitboard notABFiles = Bitboard{(uint64_t) 0x1f3e7cf9f3e7c};

    std::array<Bitboard, FILES * RANKS> lookup_table = {0};

    for (int i = 0; i < 49; ++i) {
        const Bitboard sqr(i);

        const Bitboard file = sqr | (sqr << 7) | (sqr << 14) | (sqr >> 14) | (sqr >> 7);
        const Bitboard east = ((file & notFGFiles) << 2) | ((file & notGFile) << 1);
        const Bitboard west = ((file & notABFiles) >> 2) | ((file & notAFile) >> 1);

        lookup_table[i] = (file | east | west) & ~(singlesLookup[i] | sqr);
    }

    return lookup_table;
}