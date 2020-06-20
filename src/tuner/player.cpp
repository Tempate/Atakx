#include "player.hpp"

int Player::psqtScore(const Board &board, const int side) const {
    Bitboard bb = Bitboard{(uint64_t) 1ULL};

    int score = 0;

    for (int i = 0; i < dna.size(); i++) {
        score += dna[i] * (board.stones[side] & bb).popCount();
        bb = bb << 1;
    }

    return score;
}

Player Player::reproduce() const {
    Player child{dna};
    
    for (int i = 0; i < dna.size(); i++) {
        float r = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);

        if (r < mutation_rate) {
            const int sign = 1 - 2 * (rand() % 2);
            child.dna[i] += (1 + rand() % learning_rate) * sign;
        }
    }

    return child;
}

float Player::fitness() const {
    return (float) score / games;
}