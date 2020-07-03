#include "main.hpp"
#include "lookup.hpp"
#include "board.hpp"
#include "eval.hpp"
#include "tuner/tuner.hpp"

int psqt_score(const Board &board, const int side);
int holes_score(const Board &board, const int side);
int safety_score(const Board &board, const int side);
//int density_score(const Board &board);

//std::vector<Bitboard> gen_regions(const int size);


const int stone_value = 100;

int eval(const Board &board) {
    int score = 0;

    const int material = board.stones[board.turn].popCount() - board.stones[board.turn ^ 1].popCount();

    score += material;
    score += 10 * board.turn;
    score *= stone_value;
    
    score += psqt_score(board, board.turn) - psqt_score(board, board.turn ^ 1);
    score += holes_score(board, board.turn) - holes_score(board, board.turn ^ 1);
    score += safety_score(board, board.turn) - safety_score(board, board.turn ^ 1);

    const int deviation = 75;

    if (material > 0)
        score += deviation;
    else if (material < 0)
        score -= deviation;

    return score;
}

int psqt_score(const Board &board, const int side) {
    static const std::vector<std::pair<Bitboard, int>> psqt_normal = {
        std::make_pair(Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 90),
        std::make_pair(Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 70)
    };

    static const std::vector<std::pair<Bitboard, int>> psqt_endgame = {
        std::make_pair(Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 30),
        std::make_pair(Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 10)
    };

    const auto psqt = (board.empty.popCount() > 10) ? psqt_normal : psqt_endgame;

    int score = 0;

    for (const auto &[bb, value] : psqt)
        score += value * (bb & board.stones[side]).popCount();

    return score;
}

int holes_score(const Board &board, const int side) {
    static const std::array<int, 8> penalties = {0, 25, 50, 100, 150, 250, 350, 400};

    const Bitboard bb = board.stones[side].singles() & 
                        board.stones[side ^ 1].singles() &
                        board.empty;

    int score = 0;

    for (int sqr : bb) {
        const int weak = (singlesLookup[sqr] & board.stones[side]).popCount();
        score -= penalties[weak-1];
    }

    return score;
}

int safety_score(const Board &board, const int side) {
    if (board.empty.popCount() < 10)
        return 0;

    const Bitboard singles = board.stones[side].singles();
    
    const int weaknesses = (singles & board.empty).popCount();
    const int penalty = -25;

    const int safeties = (singles & board.stones[side]).popCount();
    const int bonus = 10;

    return weaknesses * penalty + safeties * bonus;
}

/*
std::vector<Bitboard> gen_regions(const int size) {
    static const std::array<Bitboard, 7> bases = {
        Bitboard{(uint64_t) 0b1000000000000000000000000000000000000000000000000},
        Bitboard{(uint64_t) 0b1100000110000000000000000000000000000000000000000},
        Bitboard{(uint64_t) 0b1110000111000011100000000000000000000000000000000},
        Bitboard{(uint64_t) 0b1111000111100011110001111000000000000000000000000},
        Bitboard{(uint64_t) 0b1111100111110011111001111100111110000000000000000},
        Bitboard{(uint64_t) 0b1111110111111011111101111110111111011111100000000},
        Bitboard{(uint64_t) 0b1111111111111111111111111111111111111111111111111}
    };

    const int repetitions = 8 - size;
    const int count = repetitions * repetitions;

    std::vector<Bitboard> regions;
    regions.reserve(count);
    regions.assign(1, bases[size - 1]);

    for (int i = 0; i < count; i++) {
        const int shift = 1 + static_cast<int>(i % 7 == repetitions);
        regions.push_back(regions[i] << shift);
    }

    return regions;
}

int density_score(const Board &board) {
    static const std::vector<Bitboard> regions = {
        Bitboard{(uint64_t) 0b1110000111000011100000000000000000000000000000000},
        Bitboard{(uint64_t) 0b0000111000011100001110000000000000000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000000000000000000011100001110000111},
        Bitboard{(uint64_t) 0b0000000000000000000000000000111000011100001110000}
    };

    int score = 0;

    for (const Bitboard &region : regions) {
        const int mine = (board.stones[board.turn] & region).popCount();
        const int other = (board.stones[board.turn ^ 1] & region).popCount();
        const int empty = mine - other;

        if (mine > other)
            score += -10;
        else if (mine < other)
            score -= -10;        
    }

    return score;
}

int index_singles(const Board &board, const int sqr) {
    assert(sqr % 7 > 0 && sqr % 7 < 6);
    assert(sqr / 7 > 0 && sqr / 7 < 6);

    Bitboard singles = singlesLookup[sqr];
    const Bitboard singles_white = singles & board.stones[WHITE];
    const Bitboard singles_black = singles & board.stones[BLACK];

    int index = 0;

    for (int i = 0; i < singles.popCount(); i++) {
        const Bitboard bb = singles.lsbBB();
        singles.unsetLSB();

        int value;

        if (bb & singles_white)
            value = 1;
        else if (bb & singles_black)
            value = 2;
        else
            value = 0;

        index *= 3;
        index += value;
    }

    return index;
}
*/