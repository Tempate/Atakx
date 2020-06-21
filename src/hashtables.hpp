#ifndef HASHTABLES_HPP_
#define HASHTABLES_HPP_

#include <string>

#include "board.hpp"
#include "moves.hpp"

#define DEF_SIZE 128
#define DEF_ENTRIES DEF_SIZE * 1024 * 1024 / sizeof(Entry)

struct Entry {
    uint64_t key = 0;

    int depth = 0;
    int score = 0;
    int flag  = 0;
    int nodes = 0;

    Move move;

    Entry() {};

    Entry(const uint64_t key, const Move &move, const int depth, const int score, const int flag):
    key{key}, move{move}, depth{depth}, score{score}, flag{flag} {};

    Entry(const uint64_t key, const int depth, const int nodes):
        key{key}, depth{depth}, nodes{nodes} {};

    public:
    void print() const {
        std::cout << "--------------- Entry ---------------" << std::endl;
        std::cout << "Key: " << key << std::endl;
        std::cout << "Depth: " << depth << std::endl;
        std::cout << "Score: " << score << std::endl;
        std::cout << "Flag: " << flag << std::endl;
        std::cout << "Move: " << move << std::endl;
    }
};

class TT {
    private:
    std::array<Entry, DEF_ENTRIES> entries;

    public:
    TT();

    void clear();

    Entry get_entry(const uint64_t key) const;
    void save_entry(const Entry entry);

    static uint64_t gen_key(const Board &board);
    static uint64_t update_key(const Board &board, const Move &move);

    int perft(Board &board, const int depth);

    private:
    static uint64_t gen_key_for_side(const Board &board, const int side);
};

extern TT tt;

void genKey(Board &board);

#endif // #ifndef HASHTABLES_HPP_