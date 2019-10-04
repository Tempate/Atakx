#include <array>
#include <iostream>
#include <sstream>
#include <vector>

#include "bitboard.h"
#include "board.h"
#include "hashtables.h"
#include "lookup.h"
#include "main.h"
#include "moves.h"

void addMoves(std::vector<Move> &moves, Bitboard &bb, const int from,
              const int type);

Board::Board() {
    blank();
    startpos();
}

Board::Board(const std::string &fen) { fromFen(fen); }

void Board::blank() {
    pieces[BLUE] = Bitboard{};
    pieces[RED] = Bitboard{};

    empty = Bitboard{}.full();
    gaps = Bitboard{};

    key = 0;

    turn = BLUE;
    opponent = RED;
}

void Board::random() {
    empty.random();

    Bitboard bb = ~empty;

    if (bb)
        do {
            const int sqr = bb.bitScanForward();
            const int color = rand() % 2;
            pieces[color] |= Bitboard{sqr};
        } while (bb.unsetLSB());

    turn = rand() % 2;
    opponent = turn ^ 1;

    genKey(FANCY_TT);
}

std::array<Board, 8> Board::genSymmetries() {
    std::array<Board, 8> symmetries;

    symmetries[0] = *this;

    for (int i = 1; i < 4; ++i) {
        symmetries[i] = symmetries[i - 1];
        symmetries[i].pieces[BLUE] = symmetries[i - 1].pieces[BLUE].rotate();
        symmetries[i].pieces[RED] = symmetries[i - 1].pieces[RED].rotate();
        symmetries[i].empty = symmetries[i - 1].empty.rotate();
    }

    symmetries[4] = symmetries[0];
    symmetries[4].pieces[BLUE] = pieces[BLUE].flipVertically();
    symmetries[4].pieces[RED] = pieces[RED].flipVertically();
    symmetries[4].empty = empty.flipVertically();

    for (int i = 5; i < 8; ++i) {
        symmetries[i] = symmetries[i - 1];
        symmetries[i].pieces[BLUE] = symmetries[i - 1].pieces[BLUE].rotate();
        symmetries[i].pieces[RED] = symmetries[i - 1].pieces[RED].rotate();
        symmetries[i].empty = symmetries[i - 1].empty.rotate();
    }

    return symmetries;
}

void Board::startpos() { fromFen("x5o/7/7/7/7/7/o5x x"); }

// TODO: accept null blocks
void Board::fromFen(const std::string &fen) {
    int rank = RANKS - 1, file = 0;
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
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            file += c - '0';
            break;
        case 'x':
        case 'X':
        case 'b':
        case 'B':
            if (inBounds)
                pieces[BLUE] |= Bitboard{file, rank};

            turn = BLUE;
            ++file;
            break;
        case 'o':
        case 'O':
        case 'w':
        case 'W':
            if (inBounds)
                pieces[RED] |= Bitboard{file, rank};

            turn = RED;
            ++file;
            break;
        case '-':
            if (inBounds)
                gaps |= Bitboard{file, rank};

            ++file;
            break;
        }
    }

    empty = ~(pieces[BLUE] | pieces[RED] | gaps);
    opponent = turn ^ 1;

    genKey(FANCY_TT);
}

std::string Board::toFen() const {
    std::string fen;

    static const char players[2] = {'x', 'o'};

    for (int y = RANKS - 1; y >= 0; y--) {
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

                if (pieces[BLUE] & sqr)
                    fen += players[BLUE];
                else if (pieces[RED] & sqr)
                    fen += players[RED];
                else
                    fen += '-';
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
    fen += players[turn];

    return fen;
}

std::vector<Move> Board::genMoves() const {
    std::vector<Move> moves;
    moves.reserve(32);

    Bitboard bb = pieces[turn];
    Bitboard sMoves;

    if (bb)
        do {
            int sqr = bb.bitScanForward();
            sMoves |= singlesLookup[sqr] & empty;

            Bitboard dMoves = doublesLookup[sqr] & empty;
            addMoves(moves, dMoves, sqr, DOUBLE);

        } while (bb.unsetLSB());

    addMoves(moves, sMoves, 0, SINGLE);

    return moves;
}

int Board::countMoves() const {
    int nMoves = 0;

    Bitboard bb = pieces[turn];
    Bitboard sMoves;

    if (bb)
        do {
            int sqr = bb.bitScanForward();
            sMoves |= singlesLookup[sqr] & empty;

            Bitboard dMoves = doublesLookup[sqr] & empty;
            nMoves += dMoves.popCount();

        } while (bb.unsetLSB());

    nMoves += sMoves.popCount();

    return nMoves;
}

void Board::make(const Move &move) {
    if (move.type != NULL_MOVE) {
        pieces[turn] ^= Bitboard{move.to};

        if (move.type == DOUBLE)
            pieces[turn] ^= Bitboard{move.from};

        Bitboard captures = singlesLookup[move.to] & pieces[opponent];

        pieces[turn] ^= captures;
        pieces[opponent] ^= captures;

        empty = ~(pieces[BLUE] | pieces[RED] | gaps);
    }

    turn ^= 1;
    opponent ^= 1;
}

void Board::print() const {
    std::cout << std::endl;

    for (int y = RANKS - 1; y >= 0; y--) {
        for (int x = 0; x < FILES; x++) {
            Bitboard sqr(x, y);

            if (pieces[BLUE] & sqr)
                std::cout << "x ";
            else if (pieces[RED] & sqr)
                std::cout << "o ";
            else if (empty & sqr)
                std::cout << ". ";
            else
                std::cout << "* ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Turn: " << (turn == BLUE ? "blue" : "red") << "\n"
              << std::endl;
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
        return -MATE_SCORE;

    if (pieces[opponent].popCount() == 0)
        return MATE_SCORE;

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

uint64_t Board::perft(int depth) const {

    if (depth == 1)
        return countMoves();

    std::vector<Move> moves = genMoves();

    // If there are still empty squares and
    // there are no moves, pass the turn.
    /*
    if (moves.size() == 0 && empty)
        moves.emplace_back();
    */

    uint64_t nodes = 0;

    for (Move move : moves) {
        Board copy = *this;
        copy.make(move);

        nodes += copy.perft(depth - 1);
    }

    return nodes;
}

void addMoves(std::vector<Move> &moves, Bitboard &bb, const int from,
              const int type) {
    if (bb)
        do {
            const int to = bb.bitScanForward();
            moves.emplace_back(from, to, type);
        } while (bb.unsetLSB());
}