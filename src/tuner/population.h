#ifndef POPULATION_H_
#define POPULATION_H_

#include "../main.h"
#include "player.h"

#include <tuple>

class Population {
    int population_size = 10;
    std::vector<Player> population;

    public:
    Population();

    Population(std::vector<Player> population): population(population) {
        population_size = population.size();
    }

    std::pair<Player, float> calcFitness() const;
    void compete();
    void nextGeneration(const float best_fitness);
};

#endif 
