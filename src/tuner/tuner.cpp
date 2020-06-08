#include "../main.h"
#include "../alphabeta.h"
#include "../uai.h"

#include "population.h"
#include "player.h"
#include "tuner.h"

#include <fstream>
#include <tuple>

Tuner tuner{"tuned_values.txt", "ply2.txt"};

std::vector<std::string> Tuner::parseOpenings() {
    std::string line;
    std::ifstream file(openings_filename);

    std::vector<std::string> openings;
    
    if (file.is_open()) {
        while (getline(file, line))
            openings.push_back(line);

        file.close();
    }

    return openings;
}

void Tuner::run() {
    Population population;

    for (int i = 0; i < generations; i++) {
        std::cout << "Generation " << i + 1 << std::endl;
        population.compete();
        population.nextGeneration();
    }
}

int Tuner::tunePsqtScore(const Board &board, const int side) const {
    return playing[board.turn]->psqtScore(board, side);
}

void Tuner::match(Player &player1, Player &player2) {
    static const std::vector<std::string> openings = parseOpenings();
    std::string opening;

    playing[0] = &player1;
    playing[1] = &player2;

    std::cout << "Match started" << std::endl;

    for (int i = 0; i < number_games; i++) {
        player1.games++;
        player2.games++;

        if (i % 2 == 0)
            opening = openings[i % openings.size()];

        const int winner = play(opening);

        if (winner != -1)
            playing[winner]->score++;

        const auto player1 = playing[0];
        playing[0] = playing[1];
        playing[1] = player1;
    }

    std::cout << "Match ended" << std::endl;
}

int Tuner::play(const std::string &opening) {
    Board board{opening};

    while (board.empty.popCount() && board.pieces[board.turn].popCount()) {
        settings.init();
        settings.movetime = movetime;
        
        const Move move = abSearch(board);
        board.make(move);

        if (board.ply > 300)
            return -1;
    }

    int winner = board.turn ^ 1;

    if (board.empty.popCount() == 0 && board.pieces[board.turn].popCount() > 24)
        winner = board.turn;

    return winner;
}