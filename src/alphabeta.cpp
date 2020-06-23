#include <algorithm>
#include <limits>

#include "board.hpp"
#include "lookup.hpp"
#include "hashtables.hpp"
#include "moves.hpp"
#include "uai.hpp"
#include "eval.hpp"

using namespace std::chrono;
using TimePoint = high_resolution_clock::time_point;

enum { EXACT, LOWER_BOUND, UPPER_BOUND };

int alphabeta(const Board &board, Settings &settings, std::vector<Move> &pv,
              const TimePoint &end, const int depth, int alpha, int beta, const bool nullmove);

// int qsearch(const Board &board, const Move &move, const int depth, int alpha, int beta);

void sort(std::vector<Move> &moves, const Board &board);
void insertion_sort(std::vector<Move> &moves);

void info_string(const int depth, const int score, const uint64_t nodes, const double elapsed, std::vector<Move> &pv);

TimePoint time_management(const Board &board, Settings &settings, TimePoint start);

Stats stats;

Move search(const Board &board, Settings &settings) {
    std::vector<Move> pv;
    Move best_move;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    settings.nodes = 0;
    stats.ttHits = 0;

    const TimePoint start = high_resolution_clock::now();
    TimePoint end;
    
    if (settings.timed)
        end = time_management(board, settings, start);

    for (int depth = 1; depth <= settings.depth; ++depth) {

        const int score = alphabeta(board, settings, pv, end, depth, alpha, beta, false);

        const TimePoint current = high_resolution_clock::now();
        const double elapsed = duration_cast<milliseconds>(current - start).count();

        if (settings.stop)
            break;

        best_move = pv.front();

        #if TUNING == false
            info_string(depth, score, settings.nodes, elapsed, pv);
        #endif
    }

    return best_move;
}

int alphabeta(const Board &board, Settings &settings, std::vector<Move> &pv, const TimePoint &end, const int depth, int alpha, int beta, const bool nullmove) {
    if (settings.stop)
        return 0;

    if (settings.timed && settings.nodes % 4096 == 0 && high_resolution_clock::now() > end) {
        settings.stop = true;
        return 0;
    }

    settings.nodes++;

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

    std::vector<Move> childPV;

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
    } else
        sort(moves, board);

    for (int i = 0; i < moves.size(); i++) {
        const Move &move = moves[i];

        // Razoring
        if (depth == 1 && i > 3 && static_eval + move.score * stone_value < alpha)
            break;

        // Late move pruning
		// Skip quiet moves on low depths
		if (((depth <= 2 && move.score <= 2) || (depth <= 6 && move.score <= 1)) && 
            best_score > std::numeric_limits<int>::min())
            break;


        Board copy = board;
        copy.key = tt.update_key(copy, move);
        copy.make(move);

        int reduct = 1;

        // Late move reduction
        // Only quiet jumps are reduced
        if (i > 3 || move.score == 0)
            reduct += 2;

        // Extensions
        //const bool many_captures = (move.type == SINGLE && move.score >= 8) || 
        //                           (move.type == DOUBLE && move.score >= 7);

        //if (move.type == DOUBLE && move.score == 3)
        //    reduct -= 1;

        // PV Search
        const bool no_captures = (move.type == SINGLE && move.score <= 3) ||
                                 (move.type == DOUBLE && move.score <= 2);

        const int new_alpha = (i != 0 && no_captures) ? -alpha-2 : -beta;

        int score = -alphabeta(copy, settings, childPV, end, depth - reduct, new_alpha, -alpha, nullmove);

        if (score > best_score && (reduct > 1 || new_alpha != -beta))
            score = -alphabeta(copy, settings, childPV, end, depth - 1, -beta, -alpha, nullmove);

        if (score > best_score) {
            best_score = score;

            pv.assign(1, move);
            pv.insert(pv.begin() + 1, childPV.begin(), childPV.end());

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

/*
int qsearch(const Board &board, const Move &last_move, const int depth, int alpha, int beta) {
    const int standPat = eval(board);

    if (depth <= 0)
        return standPat;

    if (standPat >= beta)
        return beta;

    // Delta pruning
    if (standPat + stone_value < alpha)
    	return alpha;
    
    if (alpha < standPat)
        alpha = standPat;

    std::vector<Move> moves = board.genCaptures(last_move.to);

    if (moves.size() == 0)
        return alpha;

    sort(moves, board);

    for (const Move &move : moves) {
        if (!(singlesLookup[last_move.to] & Bitboard{move.to}))
            continue;

        if (move.score <= 8)
            break;

        Board copy = board;
        copy.key = tt.update_key(copy, move);
        copy.make(move);

        const int score = -qsearch(copy, move, depth - 1, -beta, -alpha);

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }

    return score;
}
*/

void sort(std::vector<Move> &moves, const Board &board) {
    static const std::array<int, FILES * RANKS> psqt = {
        2, 0, 0, 0, 0, 0, 2,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 2
    };

    const Entry entry = tt.get_entry(board.key);

    for (Move &move : moves) {
        if (entry.key == board.key && move == entry.move) {
            move.score = 1000;
            continue;
        }

        move.score = board.countCaptures(move);

        if (move.type == SINGLE)
            move.score += 3;
    }

    insertion_sort(moves);
}

void insertion_sort(std::vector<Move> &moves) {
    assert(moves.size() > 0);

    for (int i = 1; i < moves.size(); ++i) {
        const Move move = moves[i];
        int j = i - 1;

        while (j >= 0 && moves[j].score < move.score) {
            moves[j + 1] = moves[j];
            --j;
        }

        moves[j + 1] = move;
    }
}

TimePoint time_management(const Board &board, Settings &settings, TimePoint start) {
    high_resolution_clock::duration movetime = milliseconds(0);

    if (settings.movetime)
        return start + milliseconds(settings.movetime);

    clock_t remaining, increment;

    if (board.turn == BLACK) {
        remaining = settings.btime;
        increment = settings.binc;
    } else {
        remaining = settings.wtime;
        increment = settings.winc;
    }

    if (remaining || increment)
        movetime = milliseconds(std::min(remaining >> 2, (remaining >> 5) + increment));

    return start + movetime;
}

void info_string(const int depth, const int score, const uint64_t nodes, const double elapsed, std::vector<Move> &pv) {
    std::cout << "info depth " << depth << " score " << score << " nodes " << nodes << " time " << elapsed;

    if (elapsed > 0) {
        const long nps = 1000 * nodes / elapsed;
        std::cout << " nps " << nps;
    }

    std::cout << " ttHits " << stats.ttHits;

    std::cout << " pv";

    for (const Move &move : pv)
        std::cout << " " << move;

    std::cout << std::endl;
}
