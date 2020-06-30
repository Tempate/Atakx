#include <sstream>

#include "hashtables.hpp"
#include "lookup.hpp"
#include "uai.hpp"

#include "board.hpp"

Board::Board() {
    blank();
    startpos();
}

Board::Board(const std::string &fen) { 
    fromFen(fen); 
}

void Board::blank() {
    stones[BLACK] = Bitboard{};
    stones[WHITE] = Bitboard{};

    empty = Bitboard{}.full();
    gaps = Bitboard{};

    key = 0;

    turn = BLACK;
}

// Generates a random, empty bitboard and assigns 
// the non-empty squares randomly to each player.
// Therefore, the generated board will approximately have
// 50% empty squares, 25% blue stones and 25% red stones.
// The generated board will have no gaps.
void Board::random() {
    empty.random();

    Bitboard bb = ~empty;

    for (int sqr : bb) {
        const int color = rand() % 2;
        stones[color] |= Bitboard{sqr};
    }

    turn = rand() % 2;

    key = tt.gen_key(*this);
}

void Board::startpos() { 
    fromFen("x5o/7/7/7/7/7/o5x x 0"); 
}

void Board::fromFen(const std::string &fen) {
    int rank = RANKS - 1, file = 0;
    int inBounds = true;

    blank();

    std::string boardString;
    char turnChar;
    std::string fiftyMovesString;

    std::stringstream ss(fen);

    if (!(ss >> boardString)) {
        std::cout << "[-] Fen string is blank" << std::endl;
        exit(0);
    }

    if (!(ss >> turnChar)) {
        std::cout << "[-] No turn found on fen: " << fen << std::endl;
        exit(0);
    }

    if (ss >> fiftyMovesString)
        fiftyMoves = std::stoi(fiftyMovesString);

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
            stones[BLACK] |= Bitboard{file, rank};
            ++file;
            break;
        case 'o':
        case 'O':
        case 'w':
        case 'W':
            stones[WHITE] |= Bitboard{file, rank};
            ++file;
            break;
        case '-':
            gaps |= Bitboard{file, rank};
            ++file;
            break;
        default:
            std::cout << "[-] Unknown character on board's fen: " << c << std::endl;
            exit(0);
        }
    }

    switch (turnChar) {
    case 'x':
    case 'X':
    case 'b':
    case 'B':
        turn = BLACK;
        break;
    case 'o':
    case 'O':
    case 'w':
    case 'W':
        turn = WHITE;
        break;
    default:
        std::cout << "[-] Unknown turn character: " << turnChar << std::endl;
        exit(0);
    }

    empty = ~(stones[BLACK] | stones[WHITE] | gaps);

    key = tt.gen_key(*this);
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

                if (stones[BLACK] & sqr)
                    fen += players[BLACK];
                else if (stones[WHITE] & sqr)
                    fen += players[WHITE];
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

// The average number of moves in an ataxx position is 32.
std::vector<Move> Board::genMoves() const {
    std::vector<Move> moves;
    moves.reserve(32);

    const Bitboard bb = stones[turn];
    Bitboard sMoves;

    for (int sqr : bb) {
        sMoves |= singlesLookup[sqr] & empty;

        Bitboard dMoves = doublesLookup[sqr] & empty;

        // Add double moves
        for (int to : dMoves)
            moves.emplace_back(sqr, to, DOUBLE);
    }

    // Add single moves
    for (int to : sMoves)
        moves.emplace_back(to);

    // Adds a null move in case there are no other moves.
    if (moves.size() == 0)
        moves.emplace_back();

    return moves;
}

std::vector<Move> Board::genSingleMoves() const {
    std::vector<Move> moves;
    moves.reserve(16);

    const Bitboard sMoves = stones[turn].singles() & empty;

    for (int to : sMoves)
        moves.emplace_back(to);

    return moves;
}


bool Board::isMoveLegal(const Move &move) const {
    const Bitboard free_destination = empty & Bitboard{move.to};

    switch (move.type) {
    case SINGLE:
        return free_destination && (singlesLookup[move.to] & stones[turn]);
    case DOUBLE:
        return (free_destination & doublesLookup[move.from]) && (stones[turn] & Bitboard{move.from});
    case NULL_MOVE:
        return countMoves() == 0;
    default:
        assert(false);
        return false;
    }
}

// Counts the moves in a given position without
// adding them to the move list.
// This function is only used in perft.
int Board::countMoves() const {
    int nMoves = 0;

    Bitboard bb = stones[turn];
    Bitboard sMoves;

    for (int sqr : bb) {
        sMoves |= singlesLookup[sqr] & empty;

        Bitboard dMoves = doublesLookup[sqr] & empty;
        nMoves += dMoves.popCount();
    }

    nMoves += sMoves.popCount();

    return nMoves;
}

std::vector<Move> Board::genCaptures(const int sqr) const {
    const Bitboard bb = Bitboard{sqr};
    const Bitboard attackers = doublesLookup[sqr] & stones[turn];
    const Bitboard holes = singlesLookup[sqr] & empty;

    std::vector<Move> moves;
    Bitboard sMoves;

    for (int attacker : attackers) {
        sMoves |= singlesLookup[attacker] & holes;

        //const Bitboard dMoves = doublesLookup[sqr] & holes;

        // Add double moves
        //((for (int to : dMoves)
        //    moves.emplace_back(attacker, to, DOUBLE);
    }

    // Add single moves
    for (int to : sMoves)
        moves.emplace_back(to);

    return moves;
}

int Board::countCaptures(const Move &move) const {
    if (move.type == NULL_MOVE)
        return 0;

    const Bitboard captures = singlesLookup[move.to] & stones[turn ^ 1];
    return captures.popCount();
}

void Board::make(const Move &move) {
    const int opponent = turn ^ 1;

    int fiftyMovesCounter = 0;

    if (move.type != NULL_MOVE) {
        const Bitboard captures = singlesLookup[move.to] & stones[opponent];

        stones[turn] ^= Bitboard{move.to};

        if (move.type == DOUBLE) {
            stones[turn] ^= Bitboard{move.from};
            fiftyMovesCounter = fiftyMoves + 1;
        }

        stones[turn] ^= captures;
        stones[opponent] ^= captures;

        empty = ~(stones[BLACK] | stones[WHITE] | gaps);
    }

    fiftyMoves = fiftyMovesCounter;

    turn = opponent;
    ply++;
}

void Board::print() const {
    std::cout << std::endl;

    for (int y = RANKS - 1; y >= 0; y--) {
        for (int x = 0; x < FILES; x++) {
            Bitboard sqr(x, y);

            if (stones[BLACK] & sqr)
                std::cout << "x ";
            else if (stones[WHITE] & sqr)
                std::cout << "o ";
            else if (empty & sqr)
                std::cout << ". ";
            else
                std::cout << "* ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Turn: " << (turn == BLACK ? "blue" : "red") << "\n"
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

// Returns the state of the game: LOST, DRAWN, WON, or UNFINISHED

// When adjudication is on, it'll consider a 
// win games in which the tile difference 
// is greater than the blank tiles.
float Board::state(const bool adjudicate) const {
    if (!stones[turn])
        return LOST;
    
    if (!stones[turn ^ 1])
        return WON;

    if (adjudicate == false) {
        if (empty)
            return UNFINISHED;

        const int mine = stones[turn].popCount();
        const int other = stones[turn ^ 1].popCount();

        if (mine > other)
            return WON;
        
        if (other > mine)
            return LOST;
        
        return DRAWN;
    }

    const int mine = stones[turn].popCount();
    const int other = stones[turn ^ 1].popCount();
    const int free = empty.popCount();
    
    if (mine - free > other)
        return WON;
    
    if (other - free > mine)
        return LOST;

    if (other == mine && free == 0)
        return DRAWN;

    return UNFINISHED;
}

uint64_t Board::perft(int depth) const {
    // Bulk counting
    if (depth == 1)
        return countMoves();

    std::vector<Move> moves = genMoves();

    uint64_t nodes = 0;

    for (Move move : moves) {
        Board copy = *this;
        copy.make(move);

        nodes += copy.perft(depth - 1);
    }

    return nodes;
}
