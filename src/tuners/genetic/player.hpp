#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "../../main.hpp"
#include "../../board.hpp"

class Player {
    float mutation_rate = 0.1;
    int learning_rate = 10;

    public:
    std::array<int, FILES * RANKS> dna = {
        90,   70,  70,  70,  70,  70,  90,
        70,    0,   0,   0,   0,   0,  70,
        70,    0,   0,   0,   0,   0,  70,
        70,    0,   0,   0,   0,   0,  70,
        70,    0,   0,   0,   0,   0,  70,
        70,    0,   0,   0,   0,   0,  70,
        90,   70,  70,  70,  70,  70,  90
    };

    int score = 0;
    int games = 0;

    friend std::ostream& operator<<(std::ostream& os, const Player &player);

    public:
    Player() {};
    Player(const std::array<int, FILES * RANKS> dna): dna{dna} {};

    int psqt_score(const Board &board, const int side) const;

    Player reproduce() const;
    float fitness() const;
};

#endif