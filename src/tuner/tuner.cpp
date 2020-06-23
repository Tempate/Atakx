#include "../main.hpp"
#include "../alphabeta.hpp"
#include "../uai.hpp"

#include "population.hpp"
#include "player.hpp"
#include "tuner.hpp"

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
    std::vector<Player> best_players;
    best_players.reserve(generations + 1);
    best_players.emplace_back();

    for (int i = 0; i < generations; i++) {
        std::cout << "Generation " << i + 1 << std::endl;
        best_players.push_back(testPopulation(population));
    }

    Population best_population{best_players};

    std::cout << "Generation of winners" << std::endl;
    testPopulation(best_population);
}

Player Tuner::testPopulation(Population &population) const {
    population.compete();

    Player best_player;
    float best_fitness;
    std::tie(best_player, best_fitness) = population.calcFitness();

    std::cout << "------------------------" << std::endl;

    for (int rank = 0; rank < RANKS; rank++) {
        for (int file = 0; file < FILES; file++)
            std::cout << best_player.dna[rank * FILES + file] << " ";

        std::cout << std::endl;
    }

    std::cout << "------------------------" << std::endl;

    population.nextGeneration(best_fitness);

    return best_player;
}

int Tuner::tunePsqtScore(const Board &board, const int side) const {
    return playing[board.turn]->psqtScore(board, side);
}

void Tuner::match(Player &player1, Player &player2) {
    static const std::vector<std::string> openings = parseOpenings();
    std::string opening;

    playing[0] = &player1;
    playing[1] = &player2;

    for (int i = 0; i < number_games; i++) {
        if (i % 2 == 0)
            opening = openings[i % openings.size()];

        const int winner = play(opening);

        if (winner != -1) {
            playing[winner]->score++;
            player1.games++;
            player2.games++;
        }

        const auto player1 = playing[0];
        playing[0] = playing[1];
        playing[1] = player1;
    }
}

int Tuner::play(const std::string &opening) {
    Board board{opening};

    while (board.empty.popCount() && board.stones[board.turn].popCount()) {
        Settings settings;
        settings.movetime = movetime;
        
        const Move move = search(board, settings);
        board.make(move);

        if (board.ply > 1000)
            return -1;
    }

    int winner = board.turn ^ 1;

    if (board.empty.popCount() == 0 && board.stones[board.turn].popCount() > 24)
        winner = board.turn;

    return winner;
}