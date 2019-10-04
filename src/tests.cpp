#include <chrono>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "board.h"
#include "hashtables.h"
#include "moves.h"
#include "search.h"
#include "tests.h"

void testMatesInOne() {
    static const std::vector<std::tuple<std::string, Move>> mates = {
        std::make_tuple("7/7/7/7/7/o1x4/oo5 x", Move{"b2"}),
        std::make_tuple("7/7/7/7/1o5/ox5/1ooo3 x", Move{"c2"})};

    for (const auto &[fen, sol] : mates) {
        Board board(fen);
        board.print();

        Move move = search(board);

        std::cout << (move.to == sol.to ? "PASS" : "ERROR");
        std::cout << " " << sol.toString() << " " << move.toString()
                  << std::endl;
        std::cout << std::endl;
    }
}

void testKeySymmetries() {
    static const int N = 100000;

    int success = 0;
    int collisions = 0;

    bool exit = false;

    for (int i = 0; i < N && !exit; ++i) {
        Board board{};
        board.random();
        board.genKey(true);

        std::array<Board, 8> symmetries = board.genSymmetries();
        const uint64_t key = board.key;

        for (Board &sym : symmetries) {
            sym.genKey(true);

            if (sym.key != key) {
                board.print();
                std::cout << board.key << std::endl;

                sym.print();
                std::cout << sym.key << std::endl;

                exit = true;
                break;
            } else
                success++;
        }

        std::vector<Move> moves = board.genMoves();

        for (const Move &move : moves) {
            Board copy = board;
            copy.make(move);
            copy.genKey(true);

            if (copy.key == key) {
                std::cout << "[-] There has been a collision." << std::endl;

                board.print();
                std::cout << board.key << std::endl;

                copy.print();
                std::cout << copy.key << std::endl;

                collisions++;
            }
        }
    }

    std::cout << "Succeeded: " << success << std::endl;
    std::cout << "Collisions: " << double(collisions) / N << "%" << std::endl;
}

void testTTPerft() {
    using namespace std::chrono;

    Board board{};

    static const int depth = 7;

    std::cout << "Depth " << depth << std::endl;

    auto start = high_resolution_clock::now();
    const uint64_t normal = board.perft(depth);
    auto end = high_resolution_clock::now();

    auto elapsed = duration_cast<milliseconds>(end - start).count();

    std::cout << "Mode: perft        Nodes: " << normal << "\tTime: " << elapsed
              << std::endl;

    start = high_resolution_clock::now();
    const uint64_t ttPerft = tt.perft(board, depth, false);
    end = high_resolution_clock::now();

    elapsed = duration_cast<milliseconds>(end - start).count();

    std::cout << "Move: tt perft     Nodes: " << ttPerft
              << "\tTime: " << elapsed << std::endl;

    tt.clear();

    start = high_resolution_clock::now();
    const uint64_t ttPerftFancy = tt.perft(board, depth, true);
    end = high_resolution_clock::now();

    elapsed = duration_cast<milliseconds>(end - start).count();

    std::cout << "Mode: sym perft    Nodes: " << ttPerftFancy
              << "\tTime: " << elapsed << std::endl;
}