#include <iostream>
#include <string>

#include "board.h"
#include "lookup.h"
#include "moves.h"
#include "uai.h"
#include "tests.h"

int main() {

	settings.init();

	genLookupTables();

	uai();
	
	return 0;
}
