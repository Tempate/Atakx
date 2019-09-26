#include <iostream>

#include "board.h"
#include "lookup.h"
#include "moves.h"

int main(int argc, char **argv) {

	genLookupTables();
	
	Board board = initialBoard();
	printBoard(board);
	
	return 0;
}
