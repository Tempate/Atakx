#include <algorithm>
#include <cassert>

#include "bitboard.h"
#include "board.h"
#include "hashtables.h"
#include "main.h"

#define TURN_OFFSET 98

TT tt;

// This table has been taken from:
// http://hardy.uhasselt.be/Toga/book_format.html
static const uint64_t randomKeys[100] = {
    0x9D39247E33776D41, 0x2AF7398005AAA5C7, 0x44DB015024623547,
    0x9C15F73E62A76AE2, 0x75834465489C0C89, 0x3290AC3A203001BF,
    0x0FBBAD1F61042279, 0xE83A908FF2FB60CA, 0x0D7E765D58755C10,
    0x1A083822CEAFE02D, 0x9605D5F0E25EC3B0, 0xD021FF5CD13A2ED5,
    0x40BDF15D4A672E32, 0x011355146FD56395, 0x5DB4832046F3D9E5,
    0x239F8B2D7FF719CC, 0x05D1A1AE85B49AA1, 0x679F848F6E8FC971,
    0x7449BBFF801FED0B, 0x7D11CDB1C3B7ADF0, 0x82C7709E781EB7CC,
    0xF3218F1C9510786C, 0x331478F3AF51BBE6, 0x4BB38DE5E7219443,
    0xAA649C6EBCFD50FC, 0x8DBD98A352AFD40B, 0x87D2074B81D79217,
    0x19F3C751D3E92AE1, 0xB4AB30F062B19ABF, 0x7B0500AC42047AC4,
    0xC9452CA81A09D85D, 0x24AA6C514DA27500, 0x4C9F34427501B447,
    0x14A68FD73C910841, 0xA71B9B83461CBD93, 0x03488B95B0F1850F,
    0x637B2B34FF93C040, 0x09D1BC9A3DD90A94, 0x3575668334A1DD3B,
    0x735E2B97A4C45A23, 0x18727070F1BD400B, 0x1FCBACD259BF02E7,
    0xD310A7C2CE9B6555, 0xBF983FE0FE5D8244, 0x9F74D14F7454A824,
    0x51EBDC4AB9BA3035, 0x5C82C505DB9AB0FA, 0xFCF7FE8A3430B241,
    0x3253A729B9BA3DDE, 0x8C74C368081B3075, 0xB9BC6C87167C33E7,
    0x7EF48F2B83024E20, 0x11D505D4C351BD7F, 0x6568FCA92C76A243,
    0x4DE0B0F40F32A7B8, 0x96D693460CC37E5D, 0x42E240CB63689F2F,
    0x6D2BDCDAE2919661, 0x42880B0236E4D951, 0x5F0F4A5898171BB6,
    0x39F890F579F92F88, 0x93C5B5F47356388B, 0x63DC359D8D231B78,
    0xEC16CA8AEA98AD76, 0x5355F900C2A82DC7, 0x07FB9F855A997142,
    0x5093417AA8A7ED5E, 0x7BCBC38DA25A7F3C, 0x19FC8A768CF4B6D4,
    0x637A7780DECFC0D9, 0x8249A47AEE0E41F7, 0x79AD695501E7D1E8,
    0x14ACBAF4777D5776, 0xF145B6BECCDEA195, 0xDABF2AC8201752FC,
    0x24C3C94DF9C8D3F6, 0xBB6E2924F03912EA, 0x0CE26C0B95C980D9,
    0xA49CD132BFBF7CC4, 0xE99D662AF4243939, 0x27E6AD7891165C3F,
    0x8535F040B9744FF1, 0x54B3F4FA5F40D873, 0x72B12C32127FED2B,
    0xEE954D3C7B411F47, 0x9A85AC909A24EAA1, 0x70AC4CD9F04F21F5,
    0xF9B89D3E99A075C2, 0x87B3E2B2B5C907B1, 0xA366E5B8C54F48B8,
    0xAE4A9346CC3F7CF2, 0x1920C04D47267BBD, 0x87BF02C6B49E2AE9,
    0x092237AC237F3859, 0xFF07F64EF8ED14D0, 0x8DE8DCA9F03CC54E,
    0x9C1633264DB49C89, 0xB3F22C3D0B0B38ED, 0x390E5FB44D01144B,
    0x5BFEA5B4712768E9};

TT::TT() {
    n = size * 1024 * 1024 / sizeof(Entry);
    clear();
}

void TT::clear() {
    for (int i = 0; i < n; ++i)
        entries[i] = Entry{};
}

Entry TT::get(const uint64_t key) {
    assert(n > 0);

    return entries[key % n];
}

void TT::add(const uint64_t key, Move &bestMove, const int depth,
             const int score, const int flag) {
    assert(key != 0);
    assert(n > 0);

    entries[key % n] = Entry{key, Move{}, depth, score, flag};
}

uint64_t TT::perft(Board &board, const int depth, const bool symmetry) {
    assert(depth > 0);

    if (depth == 1)
        return board.countMoves();

    board.genKey(symmetry);
    Entry entry = get(board.key);

    if (board.key == entry.key && entry.depth == depth)
        return entry.nodes;

    uint64_t nodes = 0;

    std::vector<Move> moves = board.genMoves();

    for (const Move &move : moves) {
        Board copy = board;
        copy.make(move);
        copy.genKey(symmetry);

        nodes += perft(copy, depth - 1, symmetry);
    }

    entries[board.key % n] = Entry{board.key, depth, nodes};

    return nodes;
}

void Board::genKey(const bool symmetry) {
    key = 0;

    std::array<Bitboard, 2> bbs;

    if (symmetry) {
        std::array<std::array<Bitboard, N_SYM>, 2> symmetries =
            genBBSymmetries();

        bbs = pickSymmetry(symmetries);
    } else {
        bbs[BLUE] = pieces[BLUE];
        bbs[RED] = pieces[RED];
    }

    for (int color = BLUE; color <= RED; ++color) {
        Bitboard bb = bbs[color];
        const int offset = 49 * color;

        if (bb)
            do {
                const int sqr = bb.bitScanForward();
                key ^= randomKeys[offset + sqr];
            } while (bb.unsetLSB());
    }

    key ^= randomKeys[TURN_OFFSET + turn];
}

std::array<Bitboard, 2>
pickSymmetry(const std::array<std::array<Bitboard, N_SYM>, 2> &symmetries) {
    std::array<Bitboard, 2> min = {symmetries[BLUE][0], symmetries[RED][0]};

    int index = 0;

    for (int i = 1; i < N_SYM; ++i) {
        if ((symmetries[BLUE][i] < min[BLUE]) ||
            (symmetries[BLUE][i] == min[BLUE] &&
             symmetries[RED][i] < min[RED])) {
            min[BLUE] = symmetries[BLUE][i];
            min[RED] = symmetries[RED][i];
            index = i;
        }
    }

    return min;
}