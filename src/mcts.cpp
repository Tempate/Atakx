#include <cmath>
#include <chrono>

#include "main.h"
#include "nodes.h"

#define Cp 1 / sqrt(2)

Node& treePolicy(Node *node);
float rollout(Node &node);
void backupNegamax(Node *node, float reward);
float ucbScore(const Node *parent, const Node &child, const float cp);

Move uctSearch(const Board &board) {
    using namespace std::chrono;

    Node root(board);
    root.isRoot = true;

    const high_resolution_clock::time_point start = high_resolution_clock::now();
    const high_resolution_clock::time_point end = board.timeManagement(start);

    while (high_resolution_clock::now() < end) {
        Node &node = treePolicy(&root);
        float reward = rollout(node);

        backupNegamax(&node, reward);
    }

    #ifdef DEBUG
        root.display();
    #endif

    return root.bestChild(0)->move;
}

Node& treePolicy(Node *node) {
    while (node->board.state() == NOT_FINISHED) {
        if (node->isExpanded == false)
            return node->expand();

        node = node->bestChild(Cp);
    }

    return *node;
}

Node& Node::expand() {
    if (moves.size() == 0 && children.size() == 0)
        moves = board.genMoves();

    const Move move = moves.back();
    moves.pop_back();

    if (moves.size() == 0)
        isExpanded = true;

    Board copy = board;
    copy.make(move);

    children.emplace_back(this, board, move);

    return children.back();
}

Node* Node::bestChild(const float cp) {
    assert(children.size() > 0);

    std::vector<Node*> bestChildren;
    float highestScore = std::numeric_limits<float>::lowest();

    for (Node &child : children) {
        const int score = ucbScore(this, child, cp);

        if (score > highestScore) {
            highestScore = score;
            bestChildren.clear();
            bestChildren.push_back(&child);
        } else {
            bestChildren.push_back(&child);
        }
    }

    const int randIndex = rand() % bestChildren.size();

    return bestChildren[randIndex];
}

float rollout(Node &node) {
    Board copy = node.board;
    float state = copy.state();

    while (state == NOT_FINISHED) {
        std::vector<Move> moves = copy.genMoves();

        int randIndex = rand() % moves.size();
        const Move move = moves[randIndex];

        copy.make(move);

        state = copy.state();
    }

    return state;
}

void backupNegamax(Node *node, float reward) {
    while (node->isRoot == false) {
        node->visits++;
        node->reward += reward;

        reward = 1 - reward;
        node = node->parent;
    }

    node->visits++;
    node->reward += 1 - reward;
}

float ucbScore(const Node *parent, const Node &child, const float cp) {
    if (child.visits == 0)
        return std::numeric_limits<float>::max();
    
    const float exploitation = float(child.reward) / child.visits;
    const float exploration = cp * sqrt(2.0 * log(parent->visits) / child.visits);
    
    return exploitation + exploration;
}