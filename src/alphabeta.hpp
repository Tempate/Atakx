#ifndef SEARCH_HPP_
#define SEARCH_HPP_

#include "board.hpp"
#include "uai.hpp"

using TimePoint = std::chrono::high_resolution_clock::time_point;

Move search(const Board &board, Settings &settings);

struct SearchState {
    std::vector<Move> pv;
    std::vector<Move> previous_pv;

    uint64_t nodes = 0;

    bool stop = false;
    int timed;

    TimePoint end;

    int tt_hits = 0;
};

#endif // #ifndef SEARCH_HPP_