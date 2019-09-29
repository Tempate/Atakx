#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "main.h"
#include "board.h"
#include "uai.h"

Settings settings;

void isready(void);
void position(Board &board, const std::string &s);
void go(Board &board, const std::string &s);

pthread_t worker;
int working = 0;

void uai() {
    std::cout << "id name " << ENGINE_NAME << std::endl;
    std::cout << "id author " << ENGINE_AUTHOR << std::endl;
    std::cout << "uaiok" << std::endl;

	Board board;

    std::string cmd;
    std::string msg;

	while (1) {
        getline(std::cin, msg);
        std::stringstream ss(msg);
        ss >> cmd;

		if (cmd.compare("isready") == 0)
			isready();
		else if (cmd.compare("ucinewgame") == 0)
            board.startpos();
		else if (cmd.compare("position") == 0)
			position(board, msg.substr(9));
		else if (cmd.compare("eval") == 0)
			board.eval();
		else if (cmd.compare("go") == 0)
			go(board, msg.substr(3));
        else if (cmd.compare("print") == 0)
            board.print();
		else if (cmd.compare("quit") == 0)
			break;
	}
}

// Lets the GUI know the engine is ready. Serves as a ping.
void isready() {
    std::cout << "readyok" << std::endl;
}

/*
 * Sets the board to a certain position
 * position (startpos | fen) (moves e2e4 c7c5)?
 */
void position(Board &board, const std::string &s) {
    std::string cmd;
    std::stringstream ss(s);
    ss >> cmd;

    if (cmd.compare("startpos") == 0)
		board.startpos();
	else if (cmd.compare("fen") == 0)
        board.fromFen(s.substr(4));
	else
		board.fromFen(s);

    const size_t n = s.find("moves");

	if (n != -1)
        board.playSequence(s.substr(n + 6));
}

void go(Board &board, const std::string &s) {
    settings.init();

    if (s.compare("infinite") == 0)
        settings.depth = MAX_DEPTH;
    else if (s.compare("wtime") == 0)
        settings.wtime = std::stoi(s.substr(6));
    else if (s.compare("btime") == 0)
        settings.btime = std::stoi(s.substr(6));
    else if (s.compare("winc") == 0)
        settings.winc = std::stoi(s.substr(5));
    else if (s.compare("binc") == 0)
        settings.binc = std::stoi(s.substr(5));
    else if (s.compare("depth") == 0)
        settings.depth = std::stoi(s.substr(6));

    // bestmove(board);
}

/*
void bestmove(const Board &board) {
	Move bestMove = search(board);
    std::cout << "bestmove " << bestMove.toString() << std::endl;
}
*/

void infoString(const Board *board, const int depth, const int score, const Bitboard nodes, const int duration, std::vector<Move> pv) {
    std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " time " << duration;

	if (duration > 0)
		std::cout << " nps " << 1000 * nodes / duration;

    std::cout << " pv ";

	for (Move move : pv)
        std::cout << " " << move.toString();

    std::cout << std::endl;
}
