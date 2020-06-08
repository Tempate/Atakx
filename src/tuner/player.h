#ifndef PLAYER_H_
#define PLAYER_H_

#include "../board.h"

class Player {
    float mutation_rate = 1.0;
    int learning_rate = 20;

    public:
    std::array<int, 5> dna = {0, 0, 0, 0, 0};

    int score = 0;
    int games = 0;

    public:
    Player() {};
    Player(const std::array<int, 5> dna): dna{dna} {};

    int psqtScore(const Board &board, const int side) const;

    Player reproduce() const;
    float fitness() const;
};

#endif