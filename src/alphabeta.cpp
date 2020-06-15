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

void sort(std::vector<Move> &moves, const Board &board);
void insertionSort(std::vector<Move> &moves);

void infoString(const int depth, const int score, const uint64_t nodes,
                const double elapsed, std::vector<Move> &pv);

high_resolution_clock::time_point timeManagement(const Board &board, Settings &settings, high_resolution_clock::time_point start);

Stats stats;

Move abSearch(const Board &board, Settings &settings) {
    std::vector<Move> pv;
    Move bestMove;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    settings.nodes = 0;

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

    Entry entry = tt.get(board.key);

    // Transposition Table
    /*
    if (entry.key == board.key && entry.depth == depth) {
        #ifdef DEBUG
            ++stats.ttHits;
        #endif

        switch (entry.flag) {
        case LOWER_BOUND:
            alpha = std::max(alpha, entry.score);
            break;
        case UPPER_BOUND:
            beta = std::min(beta, entry.score);
            break;
        case EXACT:
            return entry.score;
        }

        if (alpha >= beta) {
            pv.assign(1, entry.bestMove);
            return entry.score;
        }
    }
    */

    const int staticEval = eval(board);
    const int delta = 10 * tileValue;
    const bool pvNode = beta - alpha > 1;

    std::vector<Move> childPV;

   	// Razoring
	if (depth == 1 && staticEval + delta < alpha)
        return staticEval;

    // Reverse Futility Pruning
	if (depth <= 4 && staticEval - depth * tileValue > beta)
        return staticEval;

    int bestScore = std::numeric_limits<int>::min();

    std::vector<Move> moves = board.genMoves();

    assert(moves.size() > 0);

    if (moves[0].type == NULL_MOVE) {
        const int ownPieces = board.pieces[board.turn].popCount();
        const int otherPieces = board.pieces[board.turn ^ 1].popCount();

        if (board.empty.popCount() == 0) {
            if (ownPieces > otherPieces)
                return MATE_SCORE; // + depth;
            else if (otherPieces > ownPieces)
                return -MATE_SCORE; // - depth;
            else
                return 0;
        }

        if (ownPieces == 0)
            return -MATE_SCORE; // - depth;
    } else
        sort(moves, board);

    for (const Move move : moves) {
        // Late move pruning
		// Skip quiet moves on low depths
		if (depth <= 6 && move.score <= 1 && bestScore > std::numeric_limits<int>::min())
            continue;

        Board copy = board;
        copy.make(move);
        copy.genKey();

        const int score = -alphabeta(copy, settings, childPV, end, depth - 1, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;

            pv.assign(1, move);
            pv.insert(pv.begin() + 1, childPV.begin(), childPV.end());

            if (bestScore > alpha) {
                alpha = bestScore;

                if (alpha >= beta)
                    break;
            }
        }
    }

    assert(bestScore > std::numeric_limits<int>::min());

    /*
    int flag = EXACT;

    if (bestScore <= prevAlpha)
        flag = UPPER_BOUND;
    else if (bestScore >= beta)
        flag = LOWER_BOUND;

    // Always replace the entry for the TT
    assert(pv.size() > 0);
    tt.add(board.key, pv[0], bestScore, depth, flag);
    */

   return bestScore;

    /*
    if (bestScore < 0)
        return bestScore + depth;
    else
        return bestScore - depth;
    */
}

void sort(std::vector<Move> &moves, const Board &board) {
    for (Move &move : moves) {
        move.score = board.countCaptures(move);

        if (move.type == SINGLE)
            move.score += 2;
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

    if (board.turn == BLUE) {
        remaining = settings.wtime;
        increment = settings.winc;
    } else {
        remaining = settings.btime;
        increment = settings.binc;
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

    std::cout << " pv";

    for (const Move &move : pv)
        std::cout << " " << move;

    std::cout << std::endl;
}
