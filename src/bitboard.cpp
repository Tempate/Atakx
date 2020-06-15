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
