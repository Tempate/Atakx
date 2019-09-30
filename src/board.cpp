#include <iostream>
#include <vector>
#include <sstream>

#include "main.h"
#include "bitboard.h"
#include "lookup.h"
#include "moves.h"

#include "board.h"

void addMoves(std::vector<Move> &moves, Bitboard &bb, const int from, const int type);

Board::Board() {
    blank();
    startpos();
}

Board::Board(const std::string &fen) {
    fromFen(fen);
}

void Board::blank() {
    pieces[BLUE] = Bitboard{};
    pieces[RED] = Bitboard{};

    empty = Bitboard{}.full();
    occupied = Bitboard{};
    
    key = Bitboard{};

    turn = BLUE;
    opponent = RED;
}

void Board::startpos() {
    fromFen("o5x/7/7/7/7/7/x5o w");
}

void Board::updateOccupancy() {
    occupied = pieces[BLUE] | pieces[RED];
    empty = ~occupied;
}

// TODO: accept null blocks
void Board::fromFen(const std::string &fen) {
    int rank = RANKS-1, file = 0;
    int inBounds = true;

    blank();

    for (char c : fen) {

        switch (c) {
        case ' ':
            inBounds = false;
            break;
        case '/':
            file = 0;
            --rank;
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7':
            file += c - '0';
            break;
        case 'o': case 'O': case 'w': case 'W':
            if (inBounds)
                pieces[BLUE] |= Bitboard{file, rank};
            
            turn = BLUE;
            ++file;
            break;
        case 'x': case 'X': case 'b': case 'B':
            if (inBounds)
                pieces[RED] |= Bitboard{file, rank};
            
            turn = RED;
            ++file;
            break;
        }
    }

    opponent = turn ^ 1;

    updateOccupancy();
}

std::string Board::toFen() const {
    std::string fen;

    for (int y = RANKS-1; y >= 0; y--) {
        int blanks = 0;

        for (int x = 0; x < FILES; x++) {
            Bitboard sqr(x, y);

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

std::vector<Move> Board::genMoves() const {
    std::vector<Move> moves;

    Bitboard bb = pieces[turn];
    Bitboard sMoves;

    if (bb) do {
        int sqr = bb.bitScanForward();
        sMoves |= singlesLookup[sqr] & empty;

        Bitboard dMoves = doublesLookup[sqr] & empty;
        addMoves(moves, dMoves, sqr, DOUBLE);

    } while (bb.unsetLSB());

    addMoves(moves, sMoves, 0, SINGLE);

    return moves;
}

void Board::make(const Move &move) {
    pieces[turn] ^= Bitboard{move.to};

    if (move.type == DOUBLE)
        pieces[turn] ^= Bitboard{move.from};

    Bitboard captures = singlesLookup[move.to] & pieces[opponent];
    
    pieces[turn] ^= captures;
    pieces[opponent] ^= captures;

    turn ^= 1;
    opponent ^= 1;

    updateOccupancy();
}

void Board::print() const {
    std::cout << std::endl;

    for (int y = RANKS-1; y >= 0; y--) {
        for (int x = 0; x < FILES; x++) {
            Bitboard sqr(x, y);

            if (pieces[BLUE] & sqr)
                std::cout << "o ";
            else if (pieces[RED] & sqr)
                std::cout << "x ";
            else
                std::cout << ". ";
        }

        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Turn: " << (turn == BLUE ? "Blue" : "Red") << std::endl;        
}


void Board::playSequence(const std::string &movesString) {
    std::stringstream s(movesString);
    std::string moveString;

    while (s >> moveString) {
        Move move(moveString);
        make(move);
    }
}

int Board::eval() const {
    if (pieces[turn].popCount() == 0)
        return std::numeric_limits<int>::min();

    if (pieces[opponent].popCount() == 0)
        return std::numeric_limits<int>::max();

    return pieces[turn].popCount() - pieces[opponent].popCount();
}

int Board::score() const {
    int ownPieces = pieces[turn].popCount();
    int otherPieces = pieces[opponent].popCount();

    if (ownPieces > otherPieces)
        return MATE_SCORE;
    else if (otherPieces > ownPieces)
        return -MATE_SCORE;
    else
        return 0;
}

void addMoves(std::vector<Move> &moves, Bitboard &bb, const int from, const int type) {    
    if (bb) do {
        const int to = bb.bitScanForward();
        moves.emplace_back(from, to, type);
    } while (bb.unsetLSB());
}