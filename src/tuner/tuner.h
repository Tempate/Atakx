#ifndef TUNER_H_
#define TUNER_H_

#include "../board.h"
#include "player.h"

class Tuner {
    const std::string output_filename;
    const std::string openings_filename;

    const int number_games = 10;
    const int learning_rate = 20;
    const int movetime = 20;
    const int generations = 100;

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
};

extern Tuner tuner;

#endif // #ifndef TUNER_H_