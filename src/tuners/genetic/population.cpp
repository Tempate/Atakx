#include "main.hpp"
#include "population.hpp"

#include <thread>

Population::Population() {
    Player player;

    for (int i = 0; i < population_size; i++)
        population[i] = player.reproduce();
}

Player Population::run() {
    compete();

    const std::pair<Player, float> &winner = calculate_fitness();

    const Player best_player = std::get<0>(winner);
    std::cout << best_player;

    const float best_fitness = std::get<1>(winner);
    reproduce(best_fitness);

    return best_player;
}

void Population::compete() {
    std::vector<std::thread> threads;

    for(int i = 0; i < THREADS; i++)
        threads.emplace_back([=](){ play_matches(i); });
    
    for(auto &thread : threads)
        thread.join();
}

void Population::play_matches(const int thread_id) {
    assert(THREADS > 0);

    static const PairingsArray pairings = gen_pairings();
    const int blocks = pairings.size() / THREADS;

    for (int i = blocks * thread_id; i < blocks * (thread_id + 1); i++) {
        assert(i >= 0 && i < pairings.size());

        const auto &pair = pairings[i];

        assert(pair.first >= 0 && pair.first < population.size());
        assert(pair.second >= 0 && pair.second < population.size());

        genetic_tuner.match(population[pair.first], population[pair.second]);

        std::cout << "Played match " << i << std::endl;
    }
}

void Population::reproduce(const float best_fitness) {
    assert(best_fitness > 0);

    std::vector<Player> pool;
    pool.reserve(10 * population_size);

    for (const Player &player : population) {
        float relative_fitness = player.fitness() / best_fitness;
        relative_fitness *= relative_fitness;

        for (int i = 0; i < relative_fitness * 10; i++)
            pool.push_back(player);
    }

    assert(pool.size() > 0);

    for (int i = 0; i < population_size; i++) {
        const int index = rand() % pool.size();
        population[i] = pool[index].reproduce();
        pool.erase(pool.begin() + index);
    }
}

std::pair<Player, float> Population::calculate_fitness() const {
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

    return {best_player, best_fitness};
}

PairingsArray Population::gen_pairings() const {
    PairingsArray pairings;

    int k = 0;

    for (int i = 0; i < population_size - 1; i++) {
        for (int j = i + 1; j < population_size; j++)
            pairings[k++] = {i, j};
    }

    return pairings;
}