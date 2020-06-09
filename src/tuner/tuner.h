#ifndef TUNER_H_
#define TUNER_H_

#include "../board.h"
#include "population.h"
#include "player.h"

class Tuner {
    const std::string output_filename;
    const std::string openings_filename;

    const int number_games = 50;
    const int movetime = 1;
    const int generations = 20;

    std::array<Player*, 2> playing;

    public:
    Tuner(const std::string output_filename, const std::string openings_filename):
        output_filename{output_filename}, openings_filename{openings_filename} {};

    public:
    void run();
    void match(Player &player1, Player &player2);
    int tunePsqtScore(const Board &board, const int side) const;

    private:
    std::vector<std::string> parseOpenings();
    int play(const std::string &opening);

    Player testPopulation(Population &population) const;
};

extern Tuner tuner;

#endif // #ifndef TUNER_H_