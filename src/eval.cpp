#include "main.h"
#include "lookup.h"
#include "board.h"
#include "eval.h"

#include <tuple>

int countHoles(const Board &board, const int side);
int psqtScore(const Board &board, const int side);

const int tileValue = 100;

int eval(const Board &board) {
    const int material = board.pieces[board.turn].popCount() - board.pieces[board.turn ^ 1].popCount();
    const int holes = countHoles(board, board.turn) - countHoles(board, board.turn ^ 1);
    const int psqt = psqtScore(board, board.turn) - psqtScore(board, board.turn ^ 1);

    return material * tileValue + holes + psqt;
}

int countHoles(const Board &board, const int side) {
    int holes = 0;

    for (int sqr : board.empty) {
        const Bitboard singles = singlesLookup[sqr];
        const Bitboard doubles = doublesLookup[sqr];
        const Bitboard neighborhood = singles | doubles;

        if (singles & board.pieces[side] && 
            !(neighborhood & board.pieces[side ^ 1]))
            holes++;
    }

    return holes;
}

int psqtScore(const Board &board, const int side) {
    int score;

    static const std::array<std::pair<Bitboard, int>, 4> pairs = {
        std::make_pair(Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 100),
        std::make_pair(Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 50),
        //std::make_pair(Bitboard{(uint64_t) 0b0000000011111001000100100010010001001111100000000}, -10),
        std::make_pair(Bitboard{(uint64_t) 0b0000000000000000000000001000000000000000000000000}, -10),
        std::make_pair(Bitboard{(uint64_t) 0b0000000000000000111000010100001110000000000000000}, 10)
        //std::make_pair(Bitboard{(uint64_t) 0b0000000010001000101000001000001010001000100000000}, 0)
    };

    

    for (const auto &pair : pairs) {
        Bitboard bb;
        int value;
        std::tie(bb, value) = pair;
        score += value * (board.pieces[side] & bb).popCount();
    }

    return score;
}

/*
int psqtScore(const Board &board, const int side) {
    static const std::array<int, FILES * RANKS> psqt = {
        20, 10, 10, 10, 10, 10, 20,
        10, 2, 2, 2, 2, 2, 10,
        10, 2, 0, 0, 0, 2, 10,
        10, 2, 0, 0, 0, 2, 10,
        10, 2, 0, 0, 0, 2, 10,
        10, 2, 2, 2, 2, 2, 10,
        20, 10, 10, 10, 10, 10, 20
    };

    Bitboard bb{(uint64_t) 1};
    int score = 0;

    for (const int value : psqt) {
        if (board.pieces[side] & bb)
            score += value;

        bb = bb << 1;
    }

    return score;
}
*/