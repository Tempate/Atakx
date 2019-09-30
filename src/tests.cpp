#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "board.h"
#include "moves.h"
#include "search.h"
#include "tests.h"

void testMatesInOne() {
    static const std::vector<std::tuple<std::string, Move>> mates = {
        // std::make_tuple("7/7/7/7/7/o1x4/oo5 x", Move{"b2"}),
        std::make_tuple("7/7/7/7/1o5/ox5/1ooo3 x", Move{"c2"}),
        // std::make_tuple("7/7/7/7/1o5/o2x3/1oxx3 o", Move{"c2"})
    };

    for (const auto &[fen, sol] : mates) {
        Board board(fen);
        Move move = search(board);

        board.print();
        std::cout << std::endl;
        std::cout << (move.to == sol.to ? "PASS" : "ERROR");
        std::cout << " " << sol.toString() << " " << move.toString() << std::endl;
    }
}