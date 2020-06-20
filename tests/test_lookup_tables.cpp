#include "../src/board.hpp"
#include "../src/lookup.hpp"
#include "main.hpp"


using TestCase  = std::pair<int, Bitboard>;
using TestTable = std::array<TestCase, 4>;
using LookupTable = std::array<Bitboard, 49>;

bool test_lookup_table(const TestTable &tests, const LookupTable &lookup_table);

void test_singles_lookup();
void test_doubles_lookup();
void test_neighbours_lookup();
void test_surroundings_lookup();
void test_liberties_lookup();
void test_diagonals_lookup();


void test_lookup_tables() {
    test_singles_lookup();
    test_doubles_lookup();
    test_neighbours_lookup();
    test_surroundings_lookup();
    test_liberties_lookup();
    test_diagonals_lookup();
}

bool test_lookup_table(const TestTable &tests, const LookupTable &lookup_table) {
    bool success = true;

    for (const auto &[sqr, bb] : tests) {
        if (lookup_table[sqr] != bb) {
            std::cout << "Fail for " << sqr << ": " << std::endl;
            lookup_table[sqr].print();
            
            std::cout << "instead of: " << std::endl;
            bb.print();
            
            success = false;
        }
    }

    return success;
}

void test_singles_lookup() {
    static const TestTable tests {{
        {24, Bitboard{(uint64_t) 0b0000000000000000111000010100001110000000000000000}},
        {0,  Bitboard{(uint64_t) 0b0000000000000000000000000000000000000000110000010}},
        {10, Bitboard{(uint64_t) 0b0000000000000000000000000000001110000101000011100}},
        {7,  Bitboard{(uint64_t) 0b0000000000000000000000000000000001100000100000011}}
    }};

    if (test_lookup_table(tests, singlesLookup))
        std::cout << "[+] Passed tests for the singles lookup table." << std::endl;
    else
        std::cout << "[-] Failed tests for the singles lookup table." << std::endl;
}

void test_doubles_lookup() {
    static const TestTable tests {{
        {24, Bitboard{(uint64_t) 0b0000000011111001000100100010010001001111100000000}},
        {0,  Bitboard{(uint64_t) 0b0000000000000000000000000000000011100001000000100}},
        {10, Bitboard{(uint64_t) 0b0000000000000000000000111110010001001000100100010}},
        {7,  Bitboard{(uint64_t) 0b0000000000000000000000000111000010000001000000100}}
    }};

    if (test_lookup_table(tests, doublesLookup))
        std::cout << "[+] Passed tests for the doubles lookup table." << std::endl;
    else
        std::cout << "[-] Failed tests for the doubles lookup table." << std::endl;
}

void test_neighbours_lookup() {
    static const TestTable tests {{
        {24, Bitboard{(uint64_t) 0b0000000011111001111100110110011111001111100000000}},
        {0,  Bitboard{(uint64_t) 0b0000000000000000000000000000000011100001110000110}},
        {10, Bitboard{(uint64_t) 0b0000000000000000000000111110011111001101100111110}},
        {7,  Bitboard{(uint64_t) 0b0000000000000000000000000111000011100001100000111}}
    }};

    if (test_lookup_table(tests, neighboursLookup))
        std::cout << "[+] Passed tests for the neighbours lookup table." << std::endl;
    else
        std::cout << "[-] Failed tests for the neighbours lookup table." << std::endl;    
}

void test_surroundings_lookup() {
    static const TestTable tests {{
        {24, Bitboard{(uint64_t) 0b1111111111111111111111110111111111111111111111111}},
        {0,  Bitboard{(uint64_t) 0b0000000000000000000000001111000111100011110001110}},
        {10, Bitboard{(uint64_t) 0b0000000000000011111111111111111111111101111111111}},
        {7,  Bitboard{(uint64_t) 0b0000000000000000011110001111000111100011100001111}}
    }};

    if (test_lookup_table(tests, surroundingsLookup))
        std::cout << "[+] Passed tests for the surroundings lookup table." << std::endl;
    else
        std::cout << "[-] Failed tests for the surroundings lookup table." << std::endl;    
}

void test_liberties_lookup() {
    static const TestTable tests {{
        {24, Bitboard{(uint64_t) 0b0000000000000000010000010100000100000000000000000}},
        {0,  Bitboard{(uint64_t) 0b0000000000000000000000000000000000000000010000010}},
        {10, Bitboard{(uint64_t) 0b0000000000000000000000000000000100000101000001000}},
        {7,  Bitboard{(uint64_t) 0b0000000000000000000000000000000000100000100000001}}
    }};

    if (test_lookup_table(tests, libertiesLookup))
        std::cout << "[+] Passed tests for the liberties lookup table." << std::endl;
    else
        std::cout << "[-] Failed tests for the liberties lookup table." << std::endl;    
}

void test_diagonals_lookup() {
    static const TestTable tests {{
        {24, Bitboard{(uint64_t) 0b0000000000000000101000000000001010000000000000000}},
        {0,  Bitboard{(uint64_t) 0b0000000000000000000000000000000000000000100000000}},
        {10, Bitboard{(uint64_t) 0b0000000000000000000000000000001010000000000010100}},
        {7,  Bitboard{(uint64_t) 0b0000000000000000000000000000000001000000000000010}}
    }};

    if (test_lookup_table(tests, diagonalsLookup))
        std::cout << "[+] Passed tests for the diagonals lookup table." << std::endl;
    else
        std::cout << "[-] Failed tests for the diagonals lookup table." << std::endl;    
}