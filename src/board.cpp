#include <iostream>

using std::cout;
using std::endl;

#include "main.h"
#include "board.h"

Board initialBoard(void) {
	static const std::string initial = "x5o/7/7/7/7/7/o5x w KQkq - 0 1";
	return fenToBoard(initial);
}

Board blankBoard(void) {
    Board board = { 0 };

    board.empty = ~0;

    board.turn = BLUE;
    board.opponent = RED;

	return board;
}

void updateOccupancy(Board &board) {
	board.occupied = board.pieces[BLUE] | board.pieces[RED];
	board.empty = ~board.occupied;
}

// TODO: accept null blocks
Board fenToBoard(const std::string fen) {
	int rank = RANKS-1, file = 0, i = 0;

	Board board = blankBoard();

	while (fen[i] != ' ') {
		if (fen[i] == '/') {
			file = 0;
            --rank;
		} else if (fen[i] >= '1' && fen[i] <= '7') {
			file += fen[i] - '0';
		} else {
			const uint64_t sqr = getSqr(file, rank);

            if (fen[i] == 'o')
                board.pieces[BLUE] |= sqr;
            else
                board.pieces[RED]  |= sqr;

			++file;
		}

		++i;
	}

	board.turn = (fen[++i] == 'o') ? BLUE : RED;
	board.opponent = board.turn ^ 1;

    updateOccupancy(board);

	return board;
}

std::string boardToFen(const Board &board) {
	std::string fen;

	for (int y = RANKS-1; y >= 0; y--) {
		int blanks = 0;

		for (int x = 0; x < FILES; x++) {
			const uint64_t sqr = getSqr(x,y);

			if (board.empty & sqr) {
				++blanks;
			} else {
				if (blanks > 0) {
					fen += blanks + '0';
					blanks = 0;
				}

                fen += (board.pieces[BLUE] & sqr) ? 'o' : 'x';
			}
		}

		if (blanks > 0) {
			fen += blanks + '0';
			blanks = 0;
		}

		if (y != 0)
			fen += '/';
	}

	fen += ' ';
    fen += (board.turn == BLUE) ? 'o' : 'x';

    return fen;
}

void printBoard(const Board &board) {
	cout << endl;

	for (int y = RANKS-1; y >= 0; y--) {
		for (int x = 0; x < FILES; x++) {
			const uint64_t sqr = getSqr(x,y);

			if (board.empty & sqr) {
                cout << ". ";
			} else {
				const char tile = (board.pieces[BLUE] & sqr) ? 'o' : 'x';
                cout << tile << " ";
			}
		}

        cout << endl;
	}

    const std::string turn = (board.turn == BLUE) ? "Blue" : "Red";
    
    cout << endl;
    cout << "Turn: " << turn << endl;
}

void printBB(const uint64_t bb) {
    cout << "----------------" << endl;

	for (int y = RANKS-1; y >= 0; y--) {
		for (int x = 0; x < FILES; x++) {
            const int found = (bb & getSqr(x,y)) ? 1 : 0;
            cout << found << " ";
        }

        cout << endl;
	}

    cout << "----------------" << endl;
}
