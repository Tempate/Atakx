#include <vector>
#include <cassert>
#include <limits>

#include "board.h"
#include "moves.h"
#include "uai.h"
#include "search.h"

int alphabeta(const Board &board, std::vector<Move> &pv, const int depth, int alpha, const int beta);

Move search(const Board &board) {
    std::vector<Move> pv;

    const int alpha = std::numeric_limits<int>::min();
    const int beta = std::numeric_limits<int>::max();

    for (int depth = 1; depth <= settings.depth; ++depth) {
        int score = alphabeta(board, pv, depth, alpha, beta);

        infoString(board, depth, score, settings.nodes, 0, pv);
    }

    assert(pv.size() > 0);

    return pv.front();
}

int alphabeta(const Board &board, std::vector<Move> &pv, const int depth, int alpha, const int beta) {
    settings.nodes++;
    
    if (depth == 0)
        return board.eval();

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove;
    
    std::vector<Move> moves = board.genMoves();

    if (moves.size() == 0)
        return board.score();

    for (Move move : moves) {
        Board copy = board;
        copy.make(move);

        const int score = -alphabeta(copy, pv, depth - 1, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;

            if (bestScore > alpha) {
                alpha = bestScore;

                if (alpha >= beta)
                    break;
            }
        }
    }

    assert(bestScore > std::numeric_limits<int>::min());

    pv.assign(1, bestMove);

    return bestScore;
}