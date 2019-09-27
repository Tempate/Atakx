#include <iostream>

using std::cout;
using std::endl;

#include "main.h"
#include "moves.h"
#include "lookup.h"

#include "board.h"

void addMoves(std::vector<Move> &moves, uint64_t bb, const int from, const int type, const int color);

Board::Board() {
    pieces[BLUE] = 0;
    pieces[RED] = 0;

    empty = ~0;
    occupied = 0;
    
    key = 0;

    turn = BLUE;
    opponent = RED;

    startpos();
}

void Board::startpos() {
    fromFen("x5o/7/7/7/7/7/o5x w KQkq - 0 1");
}

void Board::updateOccupancy() {
    occupied = pieces[BLUE] | pieces[RED];
    empty = ~occupied;
}

// TODO: accept null blocks
void Board::fromFen(const std::string &fen) {
    int rank = RANKS-1, file = 0;

    for (char c : fen) {

        if (rank == 0 && file == FILES) {
            turn = (c == 'o') ? BLUE : RED;
            opponent = turn ^ 1;
            break;
        }

        switch (c) {
        case '/':
            file = 0;
            --rank;
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7':
            file += c - '0';
            break;
        case 'o': case 'O': case 'w': case 'W':
            pieces[BLUE] |= getSqr(file, rank);
            ++file;
            break;
        case 'x': case 'X': case 'b': case 'B':
            pieces[RED] |= getSqr(file, rank);
            ++file;
            break;
        }
    }

    updateOccupancy();
}

std::string Board::toFen() {
    std::string fen;

    for (int y = RANKS-1; y >= 0; y--) {
        int blanks = 0;

        for (int x = 0; x < FILES; x++) {
            uint64_t sqr = getSqr(x,y);

            if (empty & sqr) {
                ++blanks;
            } else {
                if (blanks > 0) {
                    fen += blanks + '0';
                    blanks = 0;
                }

                fen += (pieces[BLUE] & sqr) ? 'o' : 'x';
            }
        }

        if (blanks > 0) {
            fen += blanks + '0';
            blanks = 0;
        }

        if (y != 0)
            fen += '/';
    }

    fen += ' ';
    fen += (turn == BLUE) ? 'o' : 'x';

    return fen;
}

std::vector<Move> Board::genMoves() {
    std::vector<Move> moves;

    uint64_t bb = pieces[turn];
    uint64_t sMoves = 0;

    if (bb) do {
        int sqr = bitScanForward(bb);
        sMoves |= singlesLookup[sqr] & empty;

        uint64_t dMoves = doublesLookup[sqr] & empty;
        addMoves(moves, dMoves, sqr, DOUBLE, turn);

    } while (unsetLSB(bb));

    addMoves(moves, sMoves, 0, SINGLE, turn);

    return moves;
}

void addMoves(std::vector<Move> &moves, uint64_t bb, const int from, const int type, const int color) {    
    if (bb) do {
        const int to = bitScanForward(bb);
        moves.emplace_back(from, to, type, color);
    } while (unsetLSB(bb));
}

void Board::make(const Move &move) {

    pieces[turn] ^= bitmask(move.to);

    if (move.type == DOUBLE)
        pieces[turn] ^= bitmask(move.from);

    uint64_t captures = singlesLookup[move.to] & pieces[opponent];
    
    pieces[turn] ^= captures;
    pieces[opponent] ^= captures;

    turn ^= 1;
    opponent ^= 1;

    updateOccupancy();
}

void Board::print() {
    cout << endl;

    for (int y = RANKS-1; y >= 0; y--) {
        for (int x = 0; x < FILES; x++) {
            uint64_t sqr = getSqr(x,y);

            if (pieces[BLUE] & sqr)
                cout << "o ";
            else if (pieces[RED] & sqr)
                cout << "x ";
            else
                cout << ". ";
        }

        cout << endl;
    }
    
    cout << endl;
    cout << "Turn: " << (turn == BLUE ? "Blue" : "Red") << endl;        
}

void printBB(const uint64_t bb) {
    cout << "----------------" << endl;

	for (int y = RANKS-1; y >= 0; y--) {
		for (int x = 0; x < FILES; x++) {
            const int found = (bb & getSqr(x,y)) ? 1 : 0;
            cout << found << " ";
        }

        cout << endl;
	}

    cout << "----------------" << endl;
}
