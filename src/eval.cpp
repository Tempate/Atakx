#include "main.hpp"
#include "lookup.hpp"
#include "board.hpp"
#include "eval.hpp"

int psqt_score(const Board &board, const int side);
int pocket_score(const Board &board, const int side);
int holes_score(const Board &board, const int side);
int safety_score(const Board &board, const int side);

const int stone_value = 100;

int eval(const Board &board) {
    int score = 0;

    const int material = board.stones[board.turn].popCount() - board.stones[board.turn ^ 1].popCount();
    score += material * stone_value;

    /* A big bonus is given when it's white's turn, the second mover. 
     * It helps equilibrate the scores and reduce fluctuations.
     * 
     * ELO: 188 +- 67
     */
    const int komi = 7 * stone_value;
    score += komi * board.turn;
    
    score += psqt_score(board, board.turn) - psqt_score(board, board.turn ^ 1);
    score += pocket_score(board, board.turn) - pocket_score(board, board.turn ^ 1);
    score += holes_score(board, board.turn) - holes_score(board, board.turn ^ 1);
    score += safety_score(board, board.turn) - safety_score(board, board.turn ^ 1);

    return score;
}

/* A bonus is given for each stone in a valuable region of the board.
 * This format is preferred to the traditional way performance-wise.
 * 
 * ELO: 119 +- 51
 */
int psqt_score(const Board &board, const int side) {
    using PsqtTable = std::array<std::pair<Bitboard, int>, 2>;

    static const PsqtTable psqt_normal {{
        {Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 90},
        {Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 70}
    }};

    static const PsqtTable psqt_endgame {{
        {Bitboard{(uint64_t) 0b1000001000000000000000000000000000000000001000001}, 30},
        {Bitboard{(uint64_t) 0b0111110100000110000011000001100000110000010111110}, 10}
    }};

    const PsqtTable &psqt = (board.empty.popCount() > 10) ? psqt_normal : psqt_endgame;

    int score = 0;

    for (const auto &[bb, bonus] : psqt)
        score += bonus * (bb & board.stones[side]).popCount();

    return score;
}

/* A pocket is a well-dominated area from which it's easy to launch attacks.
 * A fixed bonus is given for every stone near the 2x2 corner and 
 * a variable bonus is given for stones in the suburbs (3x3 corner).
 * 
 * Likewise, central regions can be dangerous, as they are very attackable,
 * so a penalty is given for every stone in them.
 * 
 * In a psqt format it'd look like so:
 * 
 *  10  10   0 -10   0  10  10
 *  10  10   0 -10   0  10  10
 *   0   0   0 -10   0   0   0
 * -10 -10 -10 -20 -10 -10 -10
 *   0   0   0 -10   0   0   0
 *  10  10   0 -10   0  10  10
 *  10  10   0 -10   0  10  10
 * 
 * It's left this way for performance.
 * 
 * ELO: 64 +- 34
 */
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

    static const PocketArray central_regions {
        Bitboard{(uint64_t) 0b0001000000100000010001111000000000000000000000000},
        Bitboard{(uint64_t) 0b0001000000100000010000001111000000000000000000000},
        Bitboard{(uint64_t) 0b0000000000000000000001111000000100000010000001000},
        Bitboard{(uint64_t) 0b0000000000000000000000001111000100000010000001000},
    };

    static const std::array<int, 5> corner_stone_bonus = {0, 10, 20, 30, 40};
    static const std::array<int, 5> corner_suburb_stone_bonus = {0, 0, 2, 5, 15};
    static const std::array<int, 8> central_stone_penalty = {0, -5, -10, -15, -20, -25, -30, -35};

    int score = 0;

    const Bitboard &mine = board.stones[side];

    for (int i = 0; i < 4; i++) {
        const int corner_stones = (mine & corner_pockets[i]).popCount();
        score += corner_stone_bonus[corner_stones];

        const int corner_suburb_stones = (mine & corner_suburbs[i]).popCount();
        score += corner_suburb_stone_bonus[corner_stones] * corner_suburb_stones;
        
        const int central_stones = (mine & central_regions[i]).popCount();
        score += central_stone_penalty[central_stones];
    }

    return score;
}

/* A hole is an empty square surrounded by stones of either side.
 * A penalty is given for holes that are surrounded by our own stones (weak)
 * and the opponent's (strong).
 * Penalties vary depending on how many weak stones there are.
 * 
 * ELO: 178 +- 53
 */
int holes_score(const Board &board, const int side) {
    static const std::array<int, 9> penalties = {0, 0, 25, 50, 100, 150, 250, 350, 400};

    const Bitboard &enemys_frontier = board.stones[side ^ 1].singles();
    const Bitboard &enemys_two_squares_away = enemys_frontier.singles();

    const Bitboard &bb = board.stones[side].singles() & 
                         (enemys_frontier | enemys_two_squares_away) &
                         board.empty;

    int score = 0;

    for (const int sqr : bb) {
        const int weak = (singlesLookup[sqr] & board.stones[side]).popCount();
        score -= penalties[weak];
    }

    return score;
}

/* Stones are safe when they are surrounded by friendly stones;
 * and they are weak when they are surrounded by empty squares.
 * 
 * ELO: 99 +- 48
 */
int safety_score(const Board &board, const int side) {
    if (board.empty.popCount() < 10)
        return 0;

    int score = 0;

    const Bitboard &singles = board.stones[side].singles();

    const int weak = (singles & board.empty).popCount();
    score += -10 * weak;

    const int safe = (singles & board.stones[side]).popCount();
    score += 10 * safe;

    return score;
}