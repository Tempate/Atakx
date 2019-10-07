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
}

// Generates a random empty board and
// assigns the other squares randomly to each player.
// Therefore, the generated board will approximately have
// 50% free squares, 25% blue pieces and 25% red pieces.
// The generated board will have no gaps.
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

    genKey(FANCY_TT);
}

void Board::startpos() { fromFen("x5o/7/7/7/7/7/o5x x"); }

// TODO: accept null blocks
void Board::fromFen(const std::string &fen) {
    int rank = RANKS - 1, file = 0;
    int inBounds = true;

    blank();

    std::string boardString;
    char turnChar;
    std::string plyString;

    std::stringstream ss(fen);
    ss >> boardString;
    ss >> turnChar;
    ss >> plyString;

    for (char c : boardString) {
        switch (c) {
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
            pieces[BLUE] |= Bitboard{file, rank};
            ++file;
            break;
        case 'o':
        case 'O':
        case 'w':
        case 'W':
            pieces[RED] |= Bitboard{file, rank};
            ++file;
            break;
        case '-':
            gaps |= Bitboard{file, rank};
            ++file;
            break;
        default:
            std::cout << "[-] Unknown character on board's fen: " << c
                      << std::endl;
            exit(0);
        }
    }

    switch (turnChar) {
    case 'x':
    case 'X':
    case 'b':
    case 'B':
        turn = BLUE;
        break;
    case 'o':
    case 'O':
    case 'w':
    case 'W':
        turn = RED;
        break;
    default:
        std::cout << "[-] Unknown turn character: " << turnChar << std::endl;
        exit(0);
    }

    ply = std::stoi(plyString);

    empty = ~(pieces[BLUE] | pieces[RED] | gaps);

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
    fen += turn;
    fen += ' ';
    fen += ply;

    return fen;
}

// The average number of moves in an ataxx
// position is 32.
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

// Counts the moves in a given position without
// adding them to the move list.
// This function is only used on perft.
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
    const int opponent = turn ^ 1;

    if (move.type != NULL_MOVE) {
        pieces[turn] ^= Bitboard{move.to};

        if (move.type == DOUBLE)
            pieces[turn] ^= Bitboard{move.from};

        Bitboard captures = singlesLookup[move.to] & pieces[opponent];

        pieces[turn] ^= captures;
        pieces[opponent] ^= captures;

        empty = ~(pieces[BLUE] | pieces[RED] | gaps);
    }

    turn = opponent;
    ++ply;
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

// Gets a list of moves in the form of a string
// and plays them out.
void Board::playSequence(const std::string &movesString) {
    std::stringstream s(movesString);
    std::string moveString;

    while (s >> moveString) {
        Move move(moveString);
        make(move);
    }
}

int Board::eval() const {
    return pieces[turn].popCount() - pieces[turn ^ 1].popCount();
}

int Board::score() const {
    int ownPieces = pieces[turn].popCount();
    int otherPieces = pieces[turn ^ 1].popCount();

    // This could be done without branching
    if (ownPieces > otherPieces)
        return MATE_SCORE;
    else if (otherPieces > ownPieces)
        return -MATE_SCORE;
    else
        return 0;
}

uint64_t Board::perft(int depth) const {
    // Bulk counting
    if (depth == 1)
        return countMoves();

    std::vector<Move> moves = genMoves();

    // If there are no moves and there are
    // empty squares, add a null move.
    if (moves.size() == 0 && empty)
        moves.emplace_back();

    uint64_t nodes = 0;

    for (Move move : moves) {
        Board copy = *this;
        copy.make(move);

        nodes += copy.perft(depth - 1);
    }

    return nodes;
}

// Generates an array of symmetrical boards.
std::array<Board, N_SYM> Board::genSymmetries() {
    std::array<Board, N_SYM> symmetries;

    std::array<std::array<Bitboard, N_SYM>, 2> bbSymmetries = genBBSymmetries();

    for (int i = 0; i < N_SYM; ++i) {
        symmetries[i].pieces[BLUE] = bbSymmetries[BLUE][i];
        symmetries[i].pieces[RED] = bbSymmetries[RED][i];

        symmetries[i].empty = ~(bbSymmetries[BLUE][i] | bbSymmetries[RED][i]);
        symmetries[i].gaps = gaps;

        symmetries[i].turn = turn;
        symmetries[i].ply = ply;
    }

    return symmetries;
}

// Generates the symmetries for both piece bitboards.
std::array<std::array<Bitboard, N_SYM>, 2> Board::genBBSymmetries() {
    std::array<std::array<Bitboard, N_SYM>, 2> symmetries;

    for (int color = BLUE; color <= RED; ++color) {
        symmetries[color][0] = pieces[color];
        symmetries[color][1] = symmetries[color][0].rotate180();
        symmetries[color][2] = pieces[color].flipDiagonally();
        symmetries[color][3] = symmetries[color][2].rotate180();
    }

    return symmetries;
}

void addMoves(std::vector<Move> &moves, Bitboard &bb, const int from,
              const int type) {
    if (bb)
        do {
            const int to = bb.bitScanForward();
            moves.emplace_back(from, to, type);
        } while (bb.unsetLSB());
}