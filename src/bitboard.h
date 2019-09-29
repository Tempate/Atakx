#ifndef BITBOARD_H_
#define BITBOARD_H_

#include <cinttypes>
#include <iostream>

#include "main.h"

class Bitboard {
    public:
        uint64_t value;

        constexpr Bitboard():
            value{0} {}

        constexpr Bitboard(uint64_t bb):
            value{bb} {}

        constexpr Bitboard(int sqr):
            value{1ULL << sqr} {}

        constexpr Bitboard(int file, int rank):
            value{1ULL << getSquare(file, rank)} {}

        static constexpr Bitboard full() {
            return Bitboard{(uint64_t) 0x1ffffffffffff};
        }

        constexpr int getSquare(int file, int rank) {
            return rank * RANKS + file;
        }

        constexpr Bitboard operator&(const Bitboard &bb) const {
            return Bitboard{value & bb.value};
        }

        constexpr Bitboard operator^(const Bitboard &bb) const {
            return Bitboard{value ^ bb.value};
        }

        constexpr Bitboard operator|(const Bitboard &bb) const {
            return Bitboard{value | bb.value};
        }

        constexpr Bitboard operator~() const {
            return Bitboard{~value} & full();
        }

        constexpr Bitboard operator<<(const int n) const {
            return Bitboard{value << n} & full();
        }

        constexpr Bitboard operator>>(const int n) const {
            return Bitboard{value >> n};
        }

        constexpr Bitboard operator^=(const Bitboard &bb) {
            value ^= bb.value;
            return *this;
        }

        constexpr Bitboard operator|=(const Bitboard &bb) {
            value |= bb.value;
            return *this;
        }

        constexpr Bitboard operator&=(const Bitboard &bb) {
            value &= bb.value;
            return *this;
        }

        constexpr bool operator==(const Bitboard &bb) const {
            return value == bb.value;
        }

        constexpr bool operator!=(const Bitboard &bb) const {
            return value != bb.value;
        }

        constexpr operator bool() const {
            return value;
        }

        constexpr inline int popCount() const {
            return __builtin_popcountll(value); 
        }

        constexpr inline int bitScanForward() const {
            return __builtin_ctzll(value);
        }

        constexpr inline int bitScanReverse() const {
            return 48 - __builtin_clzll(value); 
        }

        constexpr inline uint64_t lsbBB() const {
            return value & -value;
        }

        constexpr inline uint64_t unsetLSB() {
            return value &= value - 1;
        }

        void print() {
            std::cout << "----------------" << std::endl;

            std::cout << value << std::endl;

            for (int y = RANKS-1; y >= 0; y--) {
                for (int x = 0; x < FILES; x++) {
                    int found = (*this & Bitboard{x, y}) ? 1 : 0;
                    std::cout << found << " ";
                }

                std::cout << std::endl;
            }

            std::cout << "----------------" << std::endl;
        }
};

#endif // #ifndef BITBOARD_H_