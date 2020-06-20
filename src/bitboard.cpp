#include <random>

#include "bitboard.hpp"

void Bitboard::random() {
    const uint64_t r = RAND_MAX * rand() + rand();
    const uint64_t s = RAND_MAX * rand() + rand();
    const int t = rand() & 0xf;

    value = (r << 34) + (s << 4) + t;
    value &= Bitboard{}.full().value;
}

void Bitboard::print() const {
    std::cout << "----------------" << std::endl;

    for (int y = RANKS - 1; y >= 0; y--) {
        for (int x = 0; x < FILES; x++)
            std::cout << ((*this & Bitboard{x, y}) ? 1 : 0) << " ";

        std::cout << std::endl;
    }

    std::cout << "----------------" << std::endl;
}

Bitboard Bitboard::singles() const {
    static const Bitboard notAFileBB = Bitboard{(uint64_t) 0b1111110111111011111101111110111111011111101111110};
    static const Bitboard notGFileBB = Bitboard{(uint64_t) 0b0111111011111101111110111111011111101111110111111};

    const Bitboard thisNotAFile = *this & notAFileBB;
    const Bitboard thisNotGFile = *this & notGFileBB;

    return ((thisNotAFile >> 8) | (thisNotAFile >> 1) | (thisNotAFile << 6) |
            (thisNotGFile << 8) | (thisNotGFile << 1) | (thisNotGFile >> 6) |
            (*this << 7) | (*this >> 7)) & Bitboard{}.full();
}