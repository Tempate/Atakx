#include <algorithm>
#include <limits>

#include "board.hpp"
#include "lookup.hpp"
#include "hashtables.hpp"
#include "moves.hpp"
#include "uai.hpp"
#include "eval.hpp"

using namespace std::chrono;

enum { EXACT, LOWER_BOUND, UPPER_BOUND };

int alphabeta(const Board &board, Settings &settings, std::vector<Move> &pv,
              const high_resolution_clock::time_point &end, const int depth,
              int alpha, int beta);

// int qsearch(const Board &board, const Move &move, const int depth, int alpha, int beta);

void sort(std::vector<Move> &moves, const Board &board);
void insertionSort(std::vector<Move> &moves);

void infoString(const int depth, const int score, const uint64_t nodes,
                const double elapsed, std::vector<Move> &pv);

high_resolution_clock::time_point timeManagement(const Board &board, Settings &settings, high_resolution_clock::time_point start);

Stats stats;
//std::array<Move, 1000> killerMoves;

Move abSearch(const Board &board, Settings &settings) {
    std::vector<Move> pv;
    Move bestMove;

    //for (int i = 0; i < killerMoves.size(); i++)
    //    killerMoves[i] = Move{};

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    settings.nodes = 0;

    #ifdef DEBUG
        stats.ttHits = 0;
    #endif

    const high_resolution_clock::time_point start = high_resolution_clock::now();
    high_resolution_clock::time_point end;
    
    if (settings.timed)
        end = timeManagement(board, settings, start);

    for (int depth = 1; depth <= settings.depth; ++depth) {
        const int score = alphabeta(board, settings, pv, end, depth, alpha, beta);

        const high_resolution_clock::time_point current = high_resolution_clock::now();
        const double elapsed = duration_cast<milliseconds>(current - start).count();

        if (settings.stop)
            break;

        bestMove = pv.front();

        #if TUNING == false
            infoString(depth, score, settings.nodes, elapsed, pv);
        #endif
    }

    // std::cout << "TT Hits: " << stats.ttHits << std::endl;

    return bestMove;
}

int alphabeta(const Board &board, Settings &settings, std::vector<Move> &pv,
              const high_resolution_clock::time_point &end, const int depth,
              int alpha, int beta) {
    if (settings.stop)
        return 0;

    if (settings.timed && settings.nodes % 4096 == 0 &&
        high_resolution_clock::now() > end) {
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
    int staticEval = eval(board);

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
            if (entry.score > alpha)
                alpha = entry.score;

            break;
        case UPPER_BOUND:
            if (entry.score < beta)
                beta = entry.score;

            break;
        case EXACT:
            if (entry.depth == depth) {
                pv.assign(1, entry.move);
                return entry.score;
            }

            break;
        }

        if (alpha >= beta) {
            pv.assign(1, entry.move);
            return entry.score;
        }

        staticEval = entry.score;
    }
    */

    const int delta = 10 * stone_value;
    const int prevAlpha = alpha;

    std::vector<Move> childPV;

   	// Razoring
	if (depth == 1 && staticEval + delta < alpha)
        return staticEval;

    // Reverse Futility Pruning
	if (depth <= 4 && staticEval - depth * stone_value > beta)
        return staticEval;

    // Null move reduction
    /*
	if (verySafe) {
		static const int R = 3;
		const int bound = beta;

		makeNullMove(board, &history);
		const int score = -pvSearch(board, depth - R - 1, -bound, -bound + 1, 1);
		undoNullMove(board, &history);

		if (score >= bound)
			return pvSearch(board, depth - R, alpha, beta, 0);
	}
    */

    int bestScore = std::numeric_limits<int>::min();

    std::vector<Move> moves = board.genMoves();
    assert(moves.size() > 0);

    if (moves[0].type == NULL_MOVE) {
        const int ownPieces = board.stones[board.turn].popCount();
        const int otherPieces = board.stones[board.turn ^ 1].popCount();

        if (board.empty.popCount() == 0) {
            if (ownPieces > otherPieces)
                return MATE_SCORE;
            else if (otherPieces > ownPieces)
                return -MATE_SCORE;
            else
                return 0;
        }

        if (ownPieces == 0)
            return -MATE_SCORE;
    } else
        sort(moves, board);

    for (const Move &move : moves) {
        // Late move pruning
		// Skip quiet moves on low depths
		if (((depth <= 2 && move.score <= 2) || (depth <= 6 && move.score <= 1)) && 
            bestScore > std::numeric_limits<int>::min())
            break;

        Board copy = board;
        copy.key = tt.update_key(copy, move);
        copy.make(move);

        int reduct = 1;

        /*
        // Late move reduction
        // Only quiet moves (excluding promotions) are reduced
        if (depth >= 2 && move.score == 0)
            reduct++;
        */

        int score = -alphabeta(copy, settings, childPV, end, depth - reduct, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;

            pv.assign(1, move);
            pv.insert(pv.begin() + 1, childPV.begin(), childPV.end());

            if (bestScore > alpha) {
                alpha = bestScore;

                if (alpha >= beta) {
                    //if (board.countCaptures(move) == 0)
                    //    killerMoves[board.ply] = move;

                    break;
                }
            }
        }
    }

    assert(bestScore > std::numeric_limits<int>::min());

    int flag = EXACT;

    if (bestScore >= beta)
        flag = LOWER_BOUND;
    else if (bestScore <= prevAlpha)
        flag = UPPER_BOUND;
    else
        flag = EXACT;

    // Always replace the entry for the TT
    assert(pv.size() > 0);

    tt.save_entry(Entry{board.key, pv[0], depth, bestScore, flag});

    return bestScore;
}

/*
int qsearch(const Board &board, const Move &last_move, const int depth, int alpha, int beta) {
    const int standPat = eval(board);

    if (depth <= 0)
        return standPat;

    if (standPat >= beta)
        return beta;

    if (alpha < standPat)
        alpha = standPat;

    // Delta pruning
    if (standPat + stone_value < alpha)
    	return alpha;

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

        if (move.type == SINGLE) {
            move.score += 3;
        } else
            move.score += psqt[move.from];

        //if (killerMoves[board.ply] == move)
        //    move.score += 4;
    }

    insertionSort(moves);
}

void insertionSort(std::vector<Move> &moves) {
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

high_resolution_clock::time_point timeManagement(const Board &board, Settings &settings, high_resolution_clock::time_point start) {
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
        movetime = milliseconds(
            std::min(remaining >> 2, (remaining >> 5) + increment));

    return start + movetime;
}

void infoString(const int depth, const int score, const uint64_t nodes,
                const double elapsed, std::vector<Move> &pv) {
    std::cout << "info depth " << depth << " score " << score << " nodes "
              << nodes << " time " << elapsed;

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
