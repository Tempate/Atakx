#include <iostream>
#include <random>

#include "bitboard.h"

void Bitboard::random() {
    const uint64_t r = RAND_MAX * rand() + rand();
    const uint64_t s = RAND_MAX * rand() + rand();
    const int t = rand() & 0xf;

    value = (r << 34) + (s << 4) + t;
    value &= Bitboard{}.full().value;
}

Bitboard Bitboard::flipVertically() const {
    return ((value << 42) & 0x1fc0000000000) | ((value << 28) & 0x3f800000000) |
           ((value << 14) & 0x7f0000000) | (value & 0xfe00000) |
           ((value >> 14) & 0x1fc000) | ((value >> 28) & 0x3f80) |
           ((value >> 42));
}

Bitboard Bitboard::flipHorizontally() const {
    return ((value >> 6) & 0x40810204081) | ((value >> 4) & 0x81020408102) |
           ((value >> 2) & 0x102040810204) | (value & 0x204081020408) |
           ((value << 2) & 0x408102040810) | ((value << 4) & 0x810204081020) |
           ((value << 6) & 0x1020408102040);
}

Bitboard Bitboard::flipDiagonally() const {
    return ((value >> 36) & 0x40) | ((value >> 30) & 0x2020) |
           ((value >> 24) & 0x101010) | ((value >> 18) & 0x8080808) |
           ((value >> 12) & 0x404040404) | ((value >> 6) & 0x20202020202) |
           (value & 0x1010101010101) | ((value << 6) & 0x808080808080) |
           ((value << 12) & 0x404040404000) | ((value << 18) & 0x202020200000) |
           ((value << 24) & 0x101010000000) | ((value << 30) & 0x80800000000) |
           ((value << 36) & 0x40000000000);
}

Bitboard Bitboard::rotate90() const {
    return flipDiagonally().flipVertically();
}

Bitboard Bitboard::rotate180() const {
    return flipHorizontally().flipVertically();
}

std::array<Bitboard, 8> Bitboard::getSymmetries() const {
    std::array<Bitboard, 8> symmetries;

    symmetries[0] = *this;

    for (int i = 1; i < 4; ++i)
        symmetries[i] = symmetries[i - 1].rotate90();

    symmetries[4] = flipVertically();

    for (int i = 5; i < 8; ++i)
        symmetries[i] = symmetries[i - 1].rotate90();

    return symmetries;
}

void Bitboard::print() const {
    std::cout << "----------------" << std::endl;

    for (int y = RANKS - 1; y >= 0; y--) {
        for (int x = 0; x < FILES; x++) {
            int found = (*this & Bitboard{x, y}) ? 1 : 0;
            std::cout << found << " ";
        }

        std::cout << std::endl;
    }

    std::cout << "----------------" << std::endl;
}