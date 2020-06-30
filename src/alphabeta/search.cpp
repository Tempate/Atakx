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

SearchState state;

namespace alphabeta {

Move search(const Board &board, Settings &settings) {
    Move best_move;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    state = SearchState{};
    state.timed = settings.timed;
    state.nodes = 0;

    const TimePoint start = std::chrono::steady_clock::now();
    
    if (settings.timed)
        state.end = time_management(board, settings, start);

    for (int depth = 1; depth <= settings.depth; depth++) {
        state.depth = depth;

        std::vector<Move> pv;

        const int score = pv_search(board, pv, depth, alpha, beta);

        state.pv = pv;

        const TimePoint current = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count();

        if (state.stop)
            break;

        best_move = pv.front();

        #if TUNING == false
            info_string(depth, score, elapsed);
        #endif
    }

    return best_move;
}

int pv_search(const Board &board, std::vector<Move> &pv, int depth, int alpha, int beta) {
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
    /*
    const Entry entry = tt.get_entry(board.key);
    const bool tt_hit = entry.key == board.key;
    */
    int static_eval = eval(board);
    const int pvNode = beta - alpha > 2;

    assert(board.key != 0);

    /*
    #ifdef DEBUG
        if (tt_hit)
            stats.ttHits++;
    #endif
    */

    /*
    if (tt_hit && entry.depth >= depth) {
        assert(entry.key != 0);
        assert(entry.depth != 0);

        switch (entry.flag) {
        case LOWER_BOUND:
            if (entry.score >= beta) {
                pv.assign(1, entry.move);
                return entry.score;
            }

            break;
        case UPPER_BOUND:
            if (entry.score <= alpha) {
                pv.assign(1, entry.move);
                return entry.score;
            }

            break;
        case EXACT:
            if (entry.depth == depth) {
                pv.assign(1, entry.move);
                return entry.score;
            }

            break;
        }
    }
    */

    const int delta = 10 * stone_value;
    const int prevAlpha = alpha;

   	// Razoring
	if (depth <= 1 && static_eval + delta < alpha)
        return static_eval;

    // Reverse Futility Pruning
	if (depth <= 4 && static_eval - depth * stone_value > beta)
        return static_eval;

    // Null move reduction
    /*
	if (!pvNode && static_eval >= beta && board.empty.popCount() > 20 && !nullmove) {
		const int R = 5;

        Board copy = board;
        copy.turn ^= 1;
        
        const int null_value = -alphabeta(copy, settings, childPV, end, depth - R - 1, -beta, -beta + 2, true);

		if (null_value >= beta) {
            const int score = alphabeta(board, settings, childPV, end, depth - R, alpha, beta, false);

            if (score >= beta) {
                pv.insert(pv.begin(), childPV.begin(), childPV.end());
                return score;
            }
        }
	}
    */

    int best_score = std::numeric_limits<int>::min();

    std::vector<Move> moves = board.genMoves();
    assert(moves.size() > 0);

    if (moves[0].type == NULL_MOVE) {
        const int mine = board.stones[board.turn].popCount();
        const int other = board.stones[board.turn ^ 1].popCount();

        if (board.empty.popCount() == 0) {
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

        // Razoring
        if (depth == 1 && i > 3 && static_eval + move.score < alpha)
            break;

        // Late move pruning
		// Skip quiet moves on low depths
		if (i > 0 && ((depth <= 2 && move.score <= 200) || (depth <= 6 && move.score <= 100)))
            break;

        Board copy = board;
        copy.key = tt.update_key(copy, move);
        copy.make(move);

        int reduct = 1;

        // Late move reduction
        // Only quiet jumps are reduced
        if (i >= 2 || move.score == 0)
            reduct += 2;

        // PV Search
        const bool no_captures = move.score <= 200;

        const int new_alpha = (i > 0 && no_captures) ? -alpha-2 : -beta;

        std::vector<Move> child_pv;

        int score = -pv_search(copy, child_pv, depth - reduct, new_alpha, -alpha);

        if (score > best_score && (reduct > 1 || new_alpha != -beta))
            score = -pv_search(copy, child_pv, depth - 1, -beta, -alpha);

        if (score > best_score) {
            best_score = score;

            pv.assign(1, move);
            pv.insert(pv.begin() + 1, child_pv.begin(), child_pv.end());

            if (best_score > alpha) {
                alpha = best_score;

                if (alpha >= beta) {
                    //if (board.countCaptures(move) == 0)
                    //    killerMoves[board.ply] = move;

                    break;
                }
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

void info_string(const int depth, const int score, const double elapsed) {
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