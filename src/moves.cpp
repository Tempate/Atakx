#include "board.h"
#include "lookup.h"
#include "moves.h"

void addMoves(std::vector<Move> moves, uint64_t bb, const int from, const int type, const int color);

void makeMove(Board &board, const Move &move) {

    board.pieces[board.turn] ^= bitmask(move.to);

    if (move.type == DOUBLE)
        board.pieces[board.turn] ^= bitmask(move.from);

    const uint64_t captures = singlesLookup[move.to] & board.pieces[board.opponent];
    
    board.pieces[board.turn] ^= captures;
    board.pieces[board.opponent] ^= captures;

    board.turn ^= 1;
    board.opponent ^= 1;

    updateOccupancy(board);
}

std::vector<Move> genMoves(Board &board) {
    std::vector<Move> moves;

    uint64_t bb = board.pieces[board.turn];
    uint64_t sMoves = 0;

    if (bb) do {
        const int sqr = bitScanForward(bb);
        sMoves |= singlesLookup[sqr] & board.empty;

        const uint64_t dMoves = doublesLookup[sqr] & board.empty;
        addMoves(moves, dMoves, sqr, DOUBLE, board.turn);

    } while (unsetLSB(bb));

    addMoves(moves, sMoves, 0, SINGLE, board.turn);

    return moves;
}

void addMoves(std::vector<Move> moves, uint64_t bb, const int from, const int type, const int color) {    
    if (bb) do {
        const int to = bitScanForward(bb);
        const Move move = {.from=from, .to=to, .type=type, .color=color};
        moves.emplace_back(move);
    } while (unsetLSB(bb));
}