#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
#include <ctime>

#include "board.h"
#include "moves.h"
#include "uai.h"
#include "search.h"

int alphabeta(const Board &board, std::vector<Move> &pv, const int depth, int alpha, const int beta);

void timeManagement(const Board &board);

clock_t start;

Move search(const Board &board) {
    std::vector<Move> pv;

    const int alpha = -MATE_SCORE;
    const int beta = MATE_SCORE;

    timeManagement(board);
    start = clock();

    for (int depth = 1; depth <= settings.depth; ++depth) {
        settings.nodes = 0;

        const int score = alphabeta(board, pv, depth, alpha, beta);
        const long duration = 1000 * (clock() - start) / CLOCKS_PER_SEC;

        infoString(depth, score, settings.nodes, duration, pv);
    }

    assert(pv.size() > 0);

    return pv.front();
}

int alphabeta(const Board &board, std::vector<Move> &pv, const int depth, int alpha, const int beta) {
	if (settings.stop)
		return 0;

	if (settings.movetime && settings.nodes % 4096 == 0 && clock() - start > settings.movetime) {
        settings.stop = true;
        return 0;
    }
    
    settings.nodes++;
    
    if (depth == 0)
        return board.eval();

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove;
    
    std::vector<Move> moves = board.genMoves();

    if (moves.size() == 0) {
        if (board.empty.popCount() == 0)
            return board.score();

        // If the board is not full the player passes its turn
        moves.push_back(Move{});
    }

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
			const clock_t timeToMove = std::min(remaining >> 2, (remaining >> 5) + increment) - 20;
			settings.movetime = (timeToMove * CLOCKS_PER_SEC) / 1000;
		}
	} else {
		settings.movetime *= CLOCKS_PER_SEC / 1000;
	}
}