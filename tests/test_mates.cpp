#include "../src/board.hpp"
#include "../src/moves.hpp"
#include "../src/uai.hpp"

#include "../src/alphabeta/search.hpp"

#include "main.hpp"

#include <vector>

void test_give_mates_in_one();
void test_avoid_mates_in_one();
void test_give_mates_in_two();

bool find_best_move(const Board &board, const Move &best_move, Settings &settings);


void test_mates() {
    test_give_mates_in_one();
    test_avoid_mates_in_one();
    test_give_mates_in_two();
}

void test_give_mates_in_one() {
    Settings settings;
    settings.init();
    settings.depth = 10;
    settings.timed = false;

    std::vector<std::pair<Board, Move>> mates = {
        std::make_pair(Board{"xxxxxxx/ooooooo/xxxxxxx/ooooooo/xxxxxxx/oooooox/xoxoxo1 x 0"}, Move{"g1"}),
        std::make_pair(Board{"xxxxxxx/ooooooo/xxxxxxx/ooooooo/xxxxxxx/oooooox/xoxoxo1 o 0"}, Move{"g1"}),
        std::make_pair(Board{"7/7/7/7/6x/3o2x/6x o 0"}, Move{"d2f2"}),
        std::make_pair(Board{"7/7/7/7/6o/3x2o/6o x 0"}, Move{"d2f2"}),
        std::make_pair(Board{"x6/7/7/3o3/7/7/7 x 0"}, Move{"a7c5"}),
        std::make_pair(Board{"x6/7/7/3o3/7/7/7 o 0"}, Move{"d4b6"}),
        std::make_pair(Board{"7/7/7/3o3/7/7/6x x 0"}, Move{"g1e3"}),
        std::make_pair(Board{"7/7/7/3o3/7/7/6x o 0"}, Move{"d4f2"}),
    };

    bool success = true;

    for (const auto &[board, best_move] : mates)
        success = find_best_move(board, best_move, settings) ? success : false;

    if (success)
        std::cout << "[+] Mates in one given successfully." << std::endl;
    else
        std::cout << "[-] Failed to give mates in one." << std::endl;
}

void test_avoid_mates_in_one() {
    Settings settings;
    settings.init();
    settings.depth = 10;
    settings.timed = false;

    std::vector<std::pair<Board, Move>> mates = {
        std::make_pair(Board{"xxxxxxx/xxxxxxx/xxxxxxx/xxx----/xxx-o2/xxx-3/xxx-3 o 0"}, Move{"e3g1"}),
        std::make_pair(Board{"ooooooo/ooooooo/ooooooo/ooo----/ooo-x2/ooo-3/ooo-3 x 0"}, Move{"e3g1"}),
        std::make_pair(Board{"xxxxxxx/x-----x/x-1-1-x/1--o1-1/x-3-x/x-----x/xxxxxxx o 0"}, Move{"c5"}),
        std::make_pair(Board{"ooooooo/o-----o/o-1-1-o/1--x1-1/o-3-o/o-----o/ooooooo x 0"}, Move{"c5"}),
        std::make_pair(Board{"xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxx1oo/xxxxooo/xxxxoo1 o 0"}, Move{"e3"}),
        std::make_pair(Board{"ooooooo/ooooooo/ooooooo/ooooooo/oooo1xx/ooooxxx/ooooxx1 x 0"}, Move{"e3"})
    };

    bool success = true;

    for (const auto &[board, best_move] : mates)
        success = find_best_move(board, best_move, settings) ? success : false;

    if (success)
        std::cout << "[+] Mates in one avoided successfully." << std::endl;
    else
        std::cout << "[-] Failed to avoid mates in one." << std::endl;
}

void test_give_mates_in_two() {
    Settings settings;
    settings.init();
    settings.depth = 10;
    settings.timed = false;

    std::vector<std::pair<Board, Move>> mates = {
        std::make_pair(Board{"1xxxxxo/xxxxxxx/xxxxxxx/xxxxxxx/xxxx1ox/xxxxxxx/xxxxxxx x 0"}, Move{"a7"}),
        std::make_pair(Board{"1ooooox/ooooooo/ooooooo/ooooooo/oooo1xo/ooooooo/ooooooo o 0"}, Move{"a7"})
    };

    bool success = true;

    for (const auto &[board, best_move] : mates)
        success = find_best_move(board, best_move, settings) ? success : false;

    if (success)
        std::cout << "[+] Mates in two given successfully." << std::endl;
    else
        std::cout << "[-] Failed to give mates in two." << std::endl;
}


bool find_best_move(const Board &board, const Move &best_move, Settings &settings) {
    const Move ab_move = alphabeta::search(board, settings);
        
    if (ab_move != best_move) {
        board.print();
        std::cout << "Fail! AB: " << ab_move << " Best: " << best_move << std::endl;
        return false;
    }

    return true;
}