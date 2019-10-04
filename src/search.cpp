#include <algorithm>
#include <cassert>
#include <ctime>
#include <limits>
#include <vector>

#include "board.h"
#include "hashtables.h"
#include "moves.h"
#include "search.h"
#include "uai.h"

enum { EXACT, LOWER_BOUND, UPPER_BOUND };

int alphabeta(const Board &board, std::vector<Move> &pv, const int depth,
              int alpha, int beta);

void sort(std::vector<Move> &moves);
void insertionSort(std::vector<Move> &moves);
void timeManagement(const Board &board);

clock_t start;

Move search(const Board &board) {
    std::vector<Move> pv;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    settings.nodes = 0;

    timeManagement(board);
    start = clock();

    for (int depth = 1; depth <= settings.depth; ++depth) {
        const int score = alphabeta(board, pv, depth, alpha, beta);
        const long duration = 1000 * (clock() - start) / CLOCKS_PER_SEC;

        if (settings.stop)
            break;

        infoString(depth, score, settings.nodes, duration, pv);
    }

    assert(pv.size() > 0);

    return pv.front();
}

int alphabeta(const Board &board, std::vector<Move> &pv, const int depth,
              int alpha, int beta) {
    if (settings.stop)
        return 0;

    if (settings.movetime && settings.nodes % 4096 == 0 &&
        clock() - start > settings.movetime) {
        settings.stop = true;
        return 0;
    }

    settings.nodes++;

    if (depth == 0)
        return board.eval();

    Entry entry = tt.get(board.key);

    // Transposition Table
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

        if (alpha >= beta)
            return entry.score;
    }

    int bestScore = std::numeric_limits<int>::min();

    std::vector<Move> childPV;
    std::vector<Move> moves = board.genMoves();

    if (moves.size() == 0) {
        if (board.empty.popCount() == 0)
            return board.score();

        if (board.pieces[board.turn].popCount() == 0)
            return -MATE_SCORE;

        assert(false);

        // If the board is not full the player passes its turn
        // moves.emplace_back();
    } else {
        sort(moves);
    }

    const int prevAlpha = alpha;

    for (Move move : moves) {
        Board copy = board;
        copy.make(move);
        copy.genKey(FANCY_TT);

        const int score = -alphabeta(copy, childPV, depth - 1, -beta, -alpha);

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

    int flag = EXACT;

    if (bestScore <= prevAlpha)
        flag = UPPER_BOUND;
    else if (bestScore >= beta)
        flag = LOWER_BOUND;

    // Always replace the entry for the TT
    assert(pv.size() > 0);
    tt.add(board.key, pv[0], bestScore, depth, flag);

    return bestScore;
}

void sort(std::vector<Move> &moves) {
    for (Move move : moves) {
        switch (move.type) {
        case NULL_MOVE:
            move.score = -100;
        case SINGLE:
            move.score = 100;
        case DOUBLE:
            move.score = 0;
        }
    }

    insertionSort(moves);
}

void insertionSort(std::vector<Move> &moves) {
    for (int i = 1; i < moves.size(); ++i) {
        int j = i - 1;

        while (j >= 0 && moves[j].score < moves[i].score) {
            moves[j + 1] = moves[j];
            --j;
        }

        moves[j + 1] = moves[i];
    }
}

void timeManagement(const Board &board) {
    if (!settings.movetime) {
        clock_t remaining, increment;

        if (board.turn == BLUE) {
            remaining = settings.wtime;
            increment = settings.winc;
        } else {
            remaining = settings.btime;
            increment = settings.binc;
        }

        if (remaining || increment) {
            const clock_t timeToMove =
                std::min(remaining >> 2, (remaining >> 5) + increment) - 20;
            settings.movetime = (timeToMove * CLOCKS_PER_SEC) / 1000;
        }
    } else {
        settings.movetime *= CLOCKS_PER_SEC / 1000;
    }
}