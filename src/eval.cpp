#include "main.hpp"
#include "lookup.hpp"
#include "board.hpp"
#include "eval.hpp"
#include "tuner/tuner.hpp"

int psqt_score(const Board &board, const int side);
int holes_score(const Board &board, const int side);
int safety_score(const Board &board, const int side);

const int stone_value = 100;

int eval(const Board &board) {
    const int material = board.stones[board.turn].popCount() - board.stones[board.turn ^ 1].popCount();

    const int psqt = psqt_score(board, board.turn) - psqt_score(board, board.turn ^ 1);
    const int holes = holes_score(board, board.turn) - holes_score(board, board.turn ^ 1);
    const int safety = safety_score(board, board.turn) - safety_score(board, board.turn ^ 1);

    return stone_value * (material + 10 * board.turn) + psqt + holes + safety;
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