#include "player.h"

int Player::psqtScore(const Board &board, const int side) const {
    static const std::array<Bitboard, 5> bbs = {
        Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001},
        Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110},
        Bitboard{(uint64_t) 0b0000000011111001000100100010010001001111100000000},
        Bitboard{(uint64_t) 0b0000000000000000111000010100001110000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000000000001000000000000000000000000}
    };

    int score;

    for (int i = 0; i < bbs.size(); i++)
        score += dna[i] * (board.pieces[side] & bbs[i]).popCount();

    return score;
}

Player Player::reproduce() const {
    Player child{dna};
    
    for (int i = 0; i < dna.size(); i++) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        if (r < mutation_rate)
            child.dna[i] += learning_rate * (1 - 2 * (rand() % 2));
    }

    return child;
}

float Player::fitness() const {
    return (float) score / games;
}