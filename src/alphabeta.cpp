#include <algorithm>
#include <limits>

#include "board.h"
#include "hashtables.h"
#include "moves.h"
#include "uai.h"

using namespace std::chrono;

enum { EXACT, LOWER_BOUND, UPPER_BOUND };

int alphabeta(const Board &board, std::vector<Move> &pv,
              const high_resolution_clock::time_point &end, const int depth,
              int alpha, int beta);

void sort(std::vector<Move> &moves);
void insertionSort(std::vector<Move> &moves);

Stats stats;

Move abSearch(const Board &board) {
    std::vector<Move> pv;
    Move bestMove;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    settings.nodes = 0;

    const high_resolution_clock::time_point start =
        high_resolution_clock::now();

    high_resolution_clock::time_point end;
    
    if (settings.timed)
        end = board.timeManagement(start);

    for (int depth = 1; depth <= settings.depth; ++depth) {
        const int score = alphabeta(board, pv, end, depth, alpha, beta);

        const high_resolution_clock::time_point current =
            high_resolution_clock::now();
        const double elapsed =
            duration_cast<milliseconds>(current - start).count();

        if (settings.stop)
            break;

        bestMove = pv.front();

        infoString(depth, score, settings.nodes, elapsed, pv);
    }

    // std::cout << "TT Hits: " << stats.ttHits << std::endl;

    return bestMove;
}

int alphabeta(const Board &board, std::vector<Move> &pv,
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

    assert(moves.size() > 0);

    if (moves[0].type == NULL_MOVE) {
        if (board.empty.popCount() == 0)
            return board.score();

        if (board.pieces[board.turn].popCount() == 0)
            return -MATE_SCORE;
    } else
        sort(moves);

    const int prevAlpha = alpha;

    for (Move move : moves) {
        Board copy = board;
        copy.make(move);
        copy.genKey(FANCY_TT);

        const int score =
            -alphabeta(copy, childPV, end, depth - 1, -beta, -alpha);

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
    for (Move &move : moves) {
        switch (move.type) {
        case SINGLE:
            move.score = 100;
            break;
        case DOUBLE:
            move.score = 0;
            break;
        }
    }

    if (moves.size() > 1)
        insertionSort(moves);
}

void insertionSort(std::vector<Move> &moves) {
    assert(moves.size() > 0);

    for (int i = 1; i < moves.size(); ++i) {
        int j = i - 1;

        while (j >= 0 && moves[j].score < moves[i].score) {
            moves[j + 1] = moves[j];
            --j;
        }

        moves[j + 1] = moves[i];
    }
}
