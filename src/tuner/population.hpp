#ifndef POPULATION_HPP_
#define POPULATION_HPP_

#include "../main.hpp"
#include "player.hpp"

#include <tuple>

class Population {
    int population_size = 10;
    std::vector<Player> population;

    std::vector<std::pair<int, int>> pairings;

    public:
    Population();

    Population(std::vector<Player> population): population(population) {
        population_size = population.size();
    }

    std::pair<Player, float> calcFitness() const;
    void compete();
    void nextGeneration(const float best_fitness);

    private:
    std::vector<std::pair<int, int>> genPairings();
    void playMatches();
};

#endif 
