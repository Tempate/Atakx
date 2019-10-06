#include <iostream>
#include <string>

#include "bitboard.h"
#include "board.h"
#include "lookup.h"
#include "tests.h"
#include "uai.h"

int main() {

    settings.init();

    genLookupTables();

    uai();

    return 0;
}
