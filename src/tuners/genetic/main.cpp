#include "../../main.hpp"
#include "../../uai.hpp"
#include "../../hashtables.hpp"

#include "../../alphabeta/search.hpp"

#include "population.hpp"
#include "player.hpp"
#include "main.hpp"

#include <fstream>
#include <tuple>

GeneticTuner genetic_tuner{"tuned_values.txt", "openings/ply2.txt"};

std::vector<std::string> GeneticTuner::parse_openings() {
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

void GeneticTuner::run() {
    Population population;

    for (int i = 0; i < generations; i++) {
        std::cout << "Generation " << i + 1 << std::endl;
        population.run();
    }
}

int GeneticTuner::tune_psqt_score(const Board &board, const int side) const {
    return playing[board.turn]->psqt_score(board, side);
}

void GeneticTuner::match(Player &player1, Player &player2) {
    static const std::vector<std::string> openings = parse_openings();
    std::string opening;
    
    assert(openings.size() > 0);

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

        const auto &player1 = playing[0];
        playing[0] = playing[1];
        playing[1] = player1;
    }
}

int GeneticTuner::play(const std::string &opening) {
    Board board{opening};
    tt.clear();

    while (board.empty.popCount() && board.stones[board.turn].popCount()) {
        Settings settings;
        settings.movetime = movetime;
        settings.timed = true;
        
        const Move move = alphabeta::search(board, settings);
        board.key = tt.update_key(board, move);
        board.make(move);

        if (board.ply > 1000)
            return -1;
    }

    int winner = board.turn ^ 1;

    if (board.empty.popCount() == 0 && board.stones[board.turn].popCount() > 24)
        winner = board.turn;

    return winner;
}