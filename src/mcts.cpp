#include <cmath>
#include <chrono>
#include <iomanip>

#include "main.h"
#include "nodes.h"

Node& treePolicy(Node *node);
float rollout(Node &node);
void backupNegamax(Node *node, float reward);
float ucbScore(const Node *parent, const Node &child, const float cp);

std::vector<Move> probePV(Node *node);
void infoString(Node &root, const std::chrono::high_resolution_clock::time_point &start);

Move uctSearch(const Board &board) {
    using namespace std::chrono;

    const long N = std::numeric_limits<long>::max();

    Node root(board);
    root.isRoot = true;

    const high_resolution_clock::time_point start = high_resolution_clock::now();
    const high_resolution_clock::time_point end = board.timeManagement(start);

    for (int i = 0; i < N; i++) {

        if (i % 1024 == 0 && high_resolution_clock::now() > end)
            break;

        if (i > 0 && i % 10000 == 0)
            infoString(root, start);

        Node &node = treePolicy(&root);
        float reward = rollout(node);

        backupNegamax(&node, reward);
    }

    return root.bestChild(0)->move;
}

Node& treePolicy(Node *node) {
    static const float CP = 1 / sqrt(2);

    while (node->board.state() == NOT_FINISHED) {
        if (node->isExpanded == false)
            return node->expand();

        node = node->bestChild(CP);
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

    children.emplace_back(this, copy, move);

    return children.back();
}

Node* Node::bestChild(const float cp) {
    assert(children.size() > 0);

    std::vector<Node*> bestChildren;
    float highestScore = std::numeric_limits<float>::lowest();

    for (Node &child : children) {
        const float score = ucbScore(this, child, cp);

        if (score > highestScore) {
            highestScore = score;
            bestChildren.clear();
            bestChildren.push_back(&child);
        } else if (score == highestScore) {
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
        Move move = moves[0];

        // Give higher probability to single moves than double moves.
        if (moves.size() > 1) {
            std::vector<Move> sMoves, dMoves;

            sMoves.reserve(moves.size());
            dMoves.reserve(moves.size());

            for (const Move &move : moves) {
                switch (move.type) {
                case SINGLE:
                    sMoves.push_back(move);
                    break;
                case DOUBLE:
                    dMoves.push_back(move);
                    break;
                }
            }

            const float factor = dMoves.size() / (3 * moves.size());

            if (rand() < factor || sMoves.size() == 0)
                move = dMoves[rand() % dMoves.size()];
            else
                move = sMoves[rand() % sMoves.size()];
        }

        copy.make(move);
        state = copy.state();
    }

    if (node.board.turn == copy.turn)
        state = 1 - state;

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
    const float exploration = 2 * cp * sqrt(2.0 * log(parent->visits) / child.visits);
    
    return exploitation + exploration;
}

std::vector<Move> probePV(Node *node) {
    std::vector<Move> pv;

    while (node->isExpanded) {
        node = node->bestChild(0);
        pv.push_back(node->move);
    }

    return pv;
}

void infoString(Node &root, const std::chrono::high_resolution_clock::time_point &start) {
    using namespace std::chrono;

    const high_resolution_clock::time_point current = high_resolution_clock::now();        
    const int elapsed = duration_cast<milliseconds>(current - start).count();
    
    const float score = float(root.reward) / root.visits;

    const std::vector<Move> pv = probePV(&root);

    std::cout << std::fixed;
    std::cout << std::setprecision(4);
    std::cout << "info nodes " << root.visits << " score " << score << " time " << elapsed;

    if (elapsed > 0) {
        const long nps = 1000 * float(root.visits) / elapsed;
        std::cout << " nps " << nps;
    }

    std::cout << " pv";

    for (const Move &move : pv)
        std::cout << " " << move.toString();

    std::cout << std::endl;
}