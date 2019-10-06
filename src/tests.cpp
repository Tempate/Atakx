#include <array>
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
    static const int N = 10000000;

    int success = 0;
    int collisions = 0;

    const int nKeys = N * N_SYM;
    std::array<uint64_t, nKeys> keys;

    bool exit = false;

    for (int i = 0; i < N && !exit; ++i) {
        Board board{};
        board.random();
        board.genKey(true);

        std::array<Board, N_SYM> symmetries = board.genSymmetries();
        const uint64_t key = board.key;

        for (int j = 0; j < N_SYM; ++j) {
            symmetries[j].genKey(true);
            keys[i * N_SYM + j] = symmetries[j].key;

            if (symmetries[j].key != key) {
                board.print();

                /*
                std::array<std::array<Bitboard, N_SYM>, 2> boardSym =
                    board.genBBSymmetries();

                int index = pickSymmetry(boardSym[BLUE]);

                boardSym[BLUE][index].print();
                boardSym[RED][index].print();
                */

                std::cout << board.key << std::endl;

                symmetries[j].print();

                /*
                boardSym = symmetries[j].genBBSymmetries();

                index = pickSymmetry(boardSym[BLUE]);

                boardSym[BLUE][index].print();
                boardSym[RED][index].print();
                */

                std::cout << symmetries[j].key << "\n" << std::endl;

                exit = true;
                break;
            } else
                success++;
        }

        for (int i = 0; i < nKeys; ++i) {
            const uint64_t key = keys[i];

            for (int j = i + 1; j < nKeys; ++j)
                collisions;
        }
    }

    std::cout << "Succeeded: " << success << std::endl;
    std::cout << "Collisions: " << double(collisions) / nKeys << "%"
              << std::endl;
}

void testPickSymmetry() {
    Board board{};
    board.random();

    std::array<std::array<Bitboard, N_SYM>, 2> symmetries =
        board.genBBSymmetries();

    for (int i = 0; i < N_SYM; ++i) {
        std::cout << "blue" << std::endl;
        symmetries[BLUE][i].print();

        std::cout << "red" << std::endl;
        symmetries[RED][i].print();
    }

    std::array<Bitboard, 2> bbs = pickSymmetry(symmetries);

    std::cout << "Picked symmetries\n" << std::endl;

    std::cout << "blue" << std::endl;
    bbs[BLUE].print();

    std::cout << "red" << std::endl;
    bbs[RED].print();
}

void testTTPerft() {
    using namespace std::chrono;

    Board board{};

    static const int depth = 8;

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