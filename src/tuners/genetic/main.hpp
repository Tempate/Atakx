#ifndef TUNER_HPP_
#define TUNER_HPP_

#include "../../board.hpp"
#include "population.hpp"
#include "player.hpp"

class GeneticTuner {
    const std::string output_filename;
    const std::string openings_filename;

    const int number_games = 100;
    const int movetime = 50;
    const int generations = 10;

    std::array<Player*, 2> playing;

    public:
    GeneticTuner(const std::string output_filename, const std::string openings_filename):
        output_filename{output_filename}, openings_filename{openings_filename} {};

    void run();
    void match(Player &player1, Player &player2);
    int tune_psqt_score(const Board &board, const int side) const;

    private:
    std::vector<std::string> parse_openings();
    int play(const std::string &opening);
};

extern GeneticTuner genetic_tuner;

#endif // #ifndef TUNER_HPP_