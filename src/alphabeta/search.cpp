#include <algorithm>
#include <limits>

#include "../board.hpp"
#include "../lookup.hpp"
#include "../hashtables.hpp"
#include "../moves.hpp"
#include "../uai.hpp"
#include "../eval.hpp"

#include "sort.hpp"
#include "search.hpp"

#include "../mcts/rollouts.hpp"

enum { EXACT, LOWER_BOUND, UPPER_BOUND };


namespace alphabeta {

Move search(const Board &board, Settings &settings) {
    Move best_move;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    SearchState state;
    state.timed = settings.timed;
    state.nodes = 0;
    state.stop = false;

    const TimePoint start = std::chrono::steady_clock::now();
    
    if (settings.timed)
        state.end = time_management(board, settings, start);

    for (int depth = 1; depth <= settings.depth; depth++) {
        state.depth = depth;

        std::vector<Move> pv;

       const int score = pv_search(board, state, pv, depth, alpha, beta);

        state.pv = pv;

        const TimePoint current = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count();

        if (state.stop) {
            assert(depth > 1);
            break;
        }

        best_move = pv.front();

        #if TUNING == false
            info_string(state, depth, score, elapsed);
        #endif
    }

    return best_move;
}

int pv_search(const Board &board, SearchState &state, std::vector<Move> &pv, int depth, int alpha, int beta) {
    if (state.stop)
        return 0;

    if (state.timed && state.nodes % 4096 == 0 && std::chrono::steady_clock::now() > state.end) {
        state.stop = true;
        return 0;
    }

    state.nodes++;

    if (board.fiftyMoves >= 100)
        return 0;

    if (depth <= 0)
        return eval(board);

    // Transposition Table
    const Entry entry = tt.get_entry(board.key);
    const bool tt_hit = entry.key == board.key;
    const int pv_node = beta - alpha > 2;

    int best_score = std::numeric_limits<int>::min();

    assert(board.key != 0);

    #ifdef DEBUG
        if (tt_hit)
            state.tt_hits++;
    #endif

   int static_eval;

    if (tt_hit && entry.depth >= depth) {
        assert(entry.key != 0);
        assert(entry.depth != 0);

        static_eval = entry.score;
    } else {
        static_eval = eval(board);
    }

    const int delta = 10 * stone_value;
    const int prevAlpha = alpha;

   	// Razoring
	if (depth <= 1 && static_eval + delta < alpha)
        return static_eval;

    // Reverse Futility Pruning
    static const std::array<int, 5> margins = {0, 100, 200, 300, 400};

	if (depth <= 4 && static_eval - margins[depth] > beta)
        return static_eval;

    std::vector<Move> moves = board.genMoves();
    assert(moves.size() > 0);

    if (moves[0].type == NULL_MOVE) {
        const int mine = board.stones[board.turn].popCount();
        const int other = board.stones[board.turn ^ 1].popCount();
        const int empty = board.empty.popCount();

        if (empty == 0) {
            if (mine > other)
                return MATE_SCORE;
            else if (other > mine)
                return -MATE_SCORE;
            else
                return 0;
        }

        if (mine == 0)
            return -MATE_SCORE;
    } else {
        alphabeta::sort(board, moves);
    }

    for (int i = 0; i < moves.size(); i++) {
        const Move &move = moves[i];

        /* Razoring
         * Stop when moves are unlikely to raise alpha
         * 
         * ELO: 37 +- 25
         */
        if (i >= 4 && static_eval + move.score < alpha)
            break;

        /* Late move pruning
         * Skip low-scoring moves at low depths
         */
        if (i > 0 && depth <= 2 && move.score <= 200)
            break;

        Board new_board = board;
        new_board.key = tt.update_key(board, move);
        new_board.make(move);

        int reduct = 1;

        /* Late move reduction
         * Reduce the search-depth for quiet moves and
         * moves ordered at the end
         * 
         * ELO: 163 +- 66
         */
        const bool quiet_move = (move.type == DOUBLE && move.captures <= 1) || (move.type == SINGLE && move.captures == 0);

        if (i >= 2 && quiet_move)
            reduct += 4;
        else if (i >= 2 || move.score <= 0)
            reduct += 2;
        
        // PV Search
        // Reduce the alpha-beta window for bad moves
        int new_alpha = -beta;

        if (move.score <= 200)
            new_alpha = -alpha - 2;

        std::vector<Move> child_pv;

        int score = -pv_search(new_board, state, child_pv, depth - reduct, new_alpha, -alpha);

        if (score > best_score && (reduct > 1 || new_alpha != -beta))
            score = -pv_search(new_board, state, child_pv, depth - 1, -beta, -alpha);

        if (score > best_score) {
            best_score = score;

            pv.assign(1, move);
            pv.insert(pv.begin() + 1, child_pv.begin(), child_pv.end());

            if (best_score > alpha) {
                alpha = best_score;

                if (alpha >= beta)
                    break;
            }
        }
    }

    assert(best_score > std::numeric_limits<int>::min());

    // Calculate the flag depending on the best score
    int flag = EXACT;

    if (best_score >= beta)
        flag = LOWER_BOUND;
    else if (best_score <= prevAlpha)
        flag = UPPER_BOUND;
    else
        flag = EXACT;

    assert(pv.size() > 0);

    // Always replace the current entry
    tt.save_entry(Entry{board.key, pv[0], depth, best_score, flag});

    return best_score;
}

TimePoint time_management(const Board &board, Settings &settings, TimePoint start) {
    std::chrono::steady_clock::duration movetime = std::chrono::milliseconds(0);

    if (settings.movetime)
        return start + std::chrono::milliseconds(settings.movetime);

    clock_t remaining, increment;

    if (board.turn == BLACK) {
        remaining = settings.btime;
        increment = settings.binc;
    } else {
        remaining = settings.wtime;
        increment = settings.winc;
    }

    if (remaining || increment)
        movetime = std::chrono::milliseconds(std::min(remaining >> 2, (remaining >> 5) + increment));

    return start + movetime;
}

void info_string(const SearchState &state, const int depth, const int score, const double elapsed) {
    std::cout << "info depth " << depth << " score " << score << " nodes " << state.nodes << " time " << elapsed;

    if (elapsed > 0) {
        const long nps = 1000 * state.nodes / elapsed;
        std::cout << " nps " << nps;
    }

    std::cout << " ttHits " << state.tt_hits;

    std::cout << " pv";

    for (const Move &move : state.pv)
        std::cout << " " << move;

    std::cout << std::endl;
}

}