#include "population.hpp"
#include "tuner.hpp"


Population::Population() {
    Player player;

    for (int i = 0; i < population_size; i++)
        population.push_back(player.reproduce());
}

void Population::compete() {

    for (int i = 0; i < population.size(); i++) {
        for (int j = 0; j < population.size(); j++) {
            if (i == j)
                continue;

            tuner.match(population[i], population[j]);
        }
    }
}

std::pair<Player, float> Population::calcFitness() const {
    Player best_player;
    float best_fitness = 0;

    for (const Player &player : population) {
        const float fitness = player.fitness();

        if (fitness > best_fitness) {
            best_fitness = fitness;
            best_player = player;
        }
    }

    assert(best_fitness > 0);

    return std::make_pair(best_player, best_fitness);
}

void Population::nextGeneration(const float best_fitness) {
    std::vector<Player> pool;
    pool.reserve(10 * population_size);

    for (const Player &player : population) {
        float relative_fitness = player.fitness() / best_fitness;
        relative_fitness *= relative_fitness;

        for (int i = 0; i < relative_fitness * 10; i++)
            pool.push_back(player);
    }

    population.clear();
    assert(pool.size() > 0);

    for (int i = 0; i < population_size; i++) {
        const int index = rand() % pool.size();
        population.push_back(pool[index].reproduce());
        pool.erase(pool.begin() + index);
    }
}
