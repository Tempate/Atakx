#include <iostream>
#include <random>

#include "bitboard.h"

void Bitboard::random() {
    const uint64_t r = RAND_MAX * rand() + rand();
    const uint64_t s = RAND_MAX * rand() + rand();
    const int t = rand() & 0xf;

    value = (r << 34) + (s << 4) + t;
}

Bitboard Bitboard::flipVertically() const {
    return ((value << 42) & 0x1fc0000000000) |
           ((value << 28) & 0x003f800000000) |
           ((value << 14) & 0x00007f0000000) | (value & 0xfe00000) |
           ((value >> 14) & 0x00000001fc000) |
           ((value >> 28) & 0x0000000003f80) | ((value >> 42));
}

Bitboard Bitboard::flipDiagonally() const {
    return ((value >> 36) & 0x00000000040) | ((value >> 30) & 0x00000002020) |
           ((value >> 24) & 0x00000101010) | ((value >> 18) & 0x00008080808) |
           ((value >> 12) & 0x00404040404) | ((value >> 6) & 0x20202020202) |
           (value & 0x1010101010101) | ((value << 6) & 0x808080808080) |
           ((value << 12) & 0x404040404000) | ((value << 18) & 0x202020200000) |
           ((value << 24) & 0x101010000000) | ((value << 30) & 0x080800000000) |
           ((value << 36) & 0x040000000000);
}

Bitboard Bitboard::rotate() const {
    Bitboard x = flipDiagonally();
    return x.flipVertically();
}

std::array<Bitboard, 8> Bitboard::getSymmetries() const {
    std::array<Bitboard, 8> symmetries;

    symmetries[0] = *this;

    for (int i = 1; i < 4; ++i)
        symmetries[i] = symmetries[i - 1].rotate();

    symmetries[4] = flipVertically();

    for (int i = 5; i < 8; ++i)
        symmetries[i] = symmetries[i - 1].rotate();

    return symmetries;
}

Bitboard Bitboard::minSymmetry() const {
    Bitboard bb = *this;
    Bitboard aux = bb;

    for (int i = 0; i < 3; ++i) {
        aux = aux.rotate();
        bb = std::min(aux, bb);
    }

    aux = aux.flipVertically();
    bb = std::min(aux, bb);

    for (int i = 0; i < 3; ++i) {
        aux = aux.rotate();
        bb = std::min(aux, bb);
    }

    return bb;
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
