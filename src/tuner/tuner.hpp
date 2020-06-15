#ifndef TUNER_HPP_
#define TUNER_HPP_

#include "../board.hpp"
#include "population.hpp"
#include "player.hpp"

class Tuner {
    const std::string output_filename;
    const std::string openings_filename;

    const int number_games = 50;
    const int movetime = 1;
    const int generations = 50;

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

#endif // #ifndef TUNER_HPP_