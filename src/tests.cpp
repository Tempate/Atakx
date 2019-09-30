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
        std::make_tuple("7/7/7/7/7/o1x4/oo5 x", Move{"b2"}),
        std::make_tuple("7/7/7/7/1o5/ox5/1ooo3 x", Move{"c2"})
    };

    for (const auto &[fen, sol] : mates) {
        Board board(fen);
        board.print();

        Move move = search(board);
        
        std::cout << (move.to == sol.to ? "PASS" : "ERROR");
        std::cout << " " << sol.toString() << " " << move.toString() << std::endl;
        std::cout << std::endl;
    }
}