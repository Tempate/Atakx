#include "main.hpp"
#include "lookup.hpp"
#include "board.hpp"
#include "eval.hpp"
#include "tuner/tuner.hpp"

int psqt_score(const Board &board, const int side);
int holes_score(const Board &board, const int side);
int safety_score(const Board &board, const int side);
int pocket_score(const Board &board, const int side);

//int density_score(const Board &board);
//std::vector<Bitboard> gen_regions(const int size);


const int stone_value = 100;

int eval(const Board &board) {
    int score = 0;

    const int material = board.stones[board.turn].popCount() - board.stones[board.turn ^ 1].popCount();
    
    score += material * stone_value;

    const int komi = 7 * stone_value;
    score += komi * board.turn;
    
    score += psqt_score(board, board.turn) - psqt_score(board, board.turn ^ 1);
    score += holes_score(board, board.turn) - holes_score(board, board.turn ^ 1);
    score += safety_score(board, board.turn) - safety_score(board, board.turn ^ 1);
    score += pocket_score(board, board.turn) - pocket_score(board, board.turn ^ 1);

    int deviation = 75;

    if (material > 0)
        score += deviation;
    else if (material < 0)
        score -= deviation;

    return score;
}

int psqt_score(const Board &board, const int side) {
    static const std::vector<std::pair<Bitboard, int>> psqt_normal {{
        {Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 90},
        {Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 70}
    }};

    static const std::vector<std::pair<Bitboard, int>> psqt_endgame {{
        {Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 30},
        {Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 10}
    }};

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

int pocket_score(const Board &board, const int side) {
    using PocketArray = std::array<Bitboard, 4>;

    static const PocketArray corner_pockets {
        Bitboard{(uint64_t) 0b1100000110000000000000000000000000000000000000000},
        Bitboard{(uint64_t) 0b0000011000001100000000000000000000000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000000000000000000000011000001100000},
        Bitboard{(uint64_t) 0b0000000000000000000000000000000000000000110000011}
    };
    
    static const PocketArray corner_suburbs {
        Bitboard{(uint64_t) 0b0010000001000011100000000000000000000000000000000},
        Bitboard{(uint64_t) 0b0000100000010000001110000000000000000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000000000000000111000000100000010000},
        Bitboard{(uint64_t) 0b0000000000000000000000000000000011100001000000100},
    };

    static const PocketArray center_pockets {
        Bitboard{(uint64_t) 0b0000000000000000000000001100000110000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000011000001100000000000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000000000011000001100000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000110000011000000000000000000000000}
    };

    static const int corner_stone_bonus = 10;
    static const std::array<int, 5> corner_suburb_stone_bonus = {0, 0, 2, 5, 15};
    static const int center_stone_penalty = -5;

    int score = 0;

    for (int i = 0; i < 4; i++) {
        const int corner_stones = (board.stones[side] & corner_pockets[i]).popCount();
        const int suburb_stones = (board.stones[side] & corner_suburbs[i]).popCount();
        const int center_stones = (board.stones[side] & center_pockets[i]).popCount();

        score += corner_stones * corner_stone_bonus;
        score += corner_suburb_stone_bonus[corner_stones] * suburb_stones;
        score += center_stones * center_stone_penalty;
    }

    return score;
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
        const int shift = 1 + (size - 1) * static_cast<int>(i % 7 == repetitions);
        regions.push_back(regions[i] << shift);
    }

    return regions;
}

int density_score(const Board &board) {
    static const std::vector<Bitboard> regions = gen_regions(2);

    int score = 0;

    for (const Bitboard &region : regions) {
        const int mine = (board.stones[board.turn] & region).popCount();
        const int other = (board.stones[board.turn ^ 1] & region).popCount();
        const int empty = mine - other;

        if (empty != 3)
            continue;

        if (mine == 1)
            score += -10;
        else if (other == 1)
            score -= -10;        
    }

    return score;
}
*/

/*
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