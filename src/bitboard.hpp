#ifndef BITBOARD_HPP_
#define BITBOARD_HPP_

#include <cinttypes>

#include "main.hpp"

class BitScanner {
private:
    uint64_t data;

public:
    constexpr BitScanner(const uint64_t &value) : data{value} {}

    constexpr int operator*() const noexcept { 
        return __builtin_ctzll(data); 
    }

    constexpr BitScanner operator++() noexcept {
        data &= data - 1;
        return *this;
    }

    constexpr bool operator!=(const BitScanner &bs) const noexcept {
        return data != bs.data;
    }
};

class Bitboard {
public:
    uint64_t value;

    constexpr Bitboard() : value{0} {}

    constexpr Bitboard(uint64_t bb) : value{bb} {}

    constexpr Bitboard(int sqr) : value{1ULL << sqr} {}

    constexpr Bitboard(int file, int rank)
        : value{1ULL << getSquare(file, rank)} {}

    void random();

    void print() const;

    Bitboard singles() const;

    static constexpr Bitboard full() {
        return Bitboard{(uint64_t) 0b1111111111111111111111111111111111111111111111111};
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

    constexpr bool operator<(const Bitboard &bb) const {
        return value < bb.value;
    }

    constexpr bool operator>(const Bitboard &bb) const {
        return value > bb.value;
    }

    constexpr bool operator>(const uint64_t n) const { 
        return value > n; 
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

    constexpr BitScanner begin() const noexcept { 
        return BitScanner{value}; 
    }

    constexpr BitScanner end() const noexcept { 
        return BitScanner{0}; 
    }

    constexpr inline Bitboard lsbBB() const { 
        return Bitboard{(uint64_t) value & -value}; 
    }

    constexpr inline void unsetLSB() { 
        value &= value - 1; 
    }
};

#endif // #ifndef BITBOARD_HPP_