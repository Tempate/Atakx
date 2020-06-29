#ifndef SEARCH_HPP_
#define SEARCH_HPP_

#include "../board.hpp"
#include "../uai.hpp"

using TimePoint = std::chrono::steady_clock::time_point;

struct SearchState {
    std::vector<Move> pv;

    uint64_t nodes = 0;
    int depth;

    bool stop = false;
    int timed;

    TimePoint end;

    int tt_hits = 0;
};

namespace alphabeta {
    Move search(const Board &board, Settings &settings);

    int pv_search(const Board &board, std::vector<Move> &pv, int depth, int alpha, int beta);

    TimePoint time_management(const Board &board, Settings &settings, TimePoint start);
    void info_string(const int depth, const int score, const double elapsed);
}

#endif // #ifndef SEARCH_HPP_