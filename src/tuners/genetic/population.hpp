#ifndef POPULATION_HPP_
#define POPULATION_HPP_

#include "../../main.hpp"
#include "player.hpp"

#include <tuple>

constexpr int population_size = 10;
constexpr int pairings_size = population_size * (population_size + 1) / 2;

using PairingsArray = std::array<std::pair<int, int>, pairings_size>;

class Population {
    private:
    std::array<Player, population_size> population;

    public:
    Population();

    Population(std::array<Player, population_size> population): population(population) {}

    Player run();

    void compete();
    void reproduce(const float best_fitness);

    std::pair<Player, float> calculate_fitness() const;

    private:
    PairingsArray gen_pairings() const;
    void play_matches(const int thread_id);
};

#endif 
