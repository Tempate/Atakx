#include "../src/board.hpp"
#include "../src/hashtables.hpp"
#include "main.hpp"

void test_update_key() {
        bool success = true;

    for (int i = 0; i < 50; i++) {
        Board board;
        board.random();

        const std::vector<Move> moves = board.genMoves();

        for (const Move &move : moves) {
            Board copy = board;
            copy.key = tt.update_key(copy, move);
            copy.make(move);

            const uint64_t key = tt.gen_key(copy);

            if (copy.key != key) {
                success = false;

                std::cout << "Failed updating key from: " << std::endl;
                board.print();

                std::cout << "to:" << std::endl;
                copy.print();

                std::cout << "The real key is " << key << " while the updated key is " << copy.key << std::endl;
            }
        }
    }

    if (success)
        std::cout << "[+] Passed tests concerning key updating." << std::endl;
    else
        std::cout << "[-] Failed tests concerning key updating." << std::endl;
}