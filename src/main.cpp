#include <iostream>

#include "board.h"
#include "lookup.h"
#include "moves.h"

int main() {

	genLookupTables();
	
	Board *board = new Board();
	board->print();
	
	return 0;
}
