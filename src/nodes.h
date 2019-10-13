#ifndef NODES_H_
#define NODES_H_

#include "main.h"
#include "board.h"
#include "moves.h"

struct Node {
    Board board;

    std::vector<Move> moves;
    std::vector<Node> children;

    Node *parent;
    Move move;

    int reward = 0;
    int visits = 0;

    bool isRoot = false;
    bool isExpanded = false;

    Node() {}

    Node(const Board &board) : board(board) {}

    Node(Node *parent, const Board &board, const Move &move) : 
        parent(parent), board(board), move(move) {}

    void print() const {
        const std::string moveStr = move.toString();

        std::cout << moveStr << std::string(8 - moveStr.size(), ' ') 
        << reward << " " << visits << std::endl;
    }

    void display() const {
        board.print();

        for (const Node &child : children)
            child.print();
    }

    Node& expand();
    Node bestChild(const float cp) const;
};

#endif // #ifndef NODES_H_