#ifndef PLAYER_H_
#define PLAYER_H_

#include "../main.h"
#include "../board.h"

class Player {
    float mutation_rate = 0.15;
    int learning_rate = 15;

    public:
    std::array<int, FILES * RANKS> dna = {
        100,  50,  50,  50,  50,  50, 100,
        50,    0,   0,   0,   0,   0,  50,
        50,    0,   0,   0,   0,   0,  50,
        50,    0,   0, -50,   0,   0,  50,
        50,    0,   0,   0,   0,   0,  50,
        50,    0,   0,   0,   0,   0,  50,
        100,  50,  50,  50,  50,  50, 100
    };

    int score = 0;
    int games = 0;

    public:
    Player() {};
    Player(const std::array<int, FILES * RANKS> dna): dna{dna} {};

    int psqtScore(const Board &board, const int side) const;

    Player reproduce() const;
    float fitness() const;
};

#endif