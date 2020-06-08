#ifndef POPULATION_H_
#define POPULATION_H_

#include "../main.h"
#include "player.h"

#include <tuple>

class Population {
    const int population_size = 10;
    std::vector<Player> population;

    public:
    Population();

    std::pair<Player, float> calcFitness() const;
    void compete();
    void nextGeneration();
};

#endif 
