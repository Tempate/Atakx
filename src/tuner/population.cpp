#include "population.h"
#include "tuner.h"


Population::Population() {
    Player player;

    for (int i = 0; i < population_size; i++)
        population.push_back(player.reproduce());
}

void Population::compete() {
    for (int i = 0; i < population.size(); i++) {
        Player &player = population[i];

        std::vector<Player> enemies = population;
        enemies.erase(enemies.begin() + i);

        for (int j = 0; j < 4; j++) {
            Player &enemy = enemies[rand() % enemies.size()];
            tuner.match(player, enemy);
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

void Population::nextGeneration() {
    Player best_player;
    float best_fitness;
    std::tie(best_player, best_fitness) = calcFitness();

    std::cout << "------------------------" << std::endl;

    for (const int value : best_player.dna)
        std::cout << value << std::endl;

    std::cout << "------------------------" << std::endl;

    std::vector<Player> pool;

    for (const Player &player : population) {
        const float relative_fitness = player.fitness() / best_fitness;

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
