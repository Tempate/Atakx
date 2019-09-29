#include <iostream>
#include <string>

#include "board.h"
#include "lookup.h"
#include "moves.h"
#include "uai.h"

int main() {

	genLookupTables();
	
	uai();
	
	return 0;
}
