#include <sstream>
#include <string>

#include "board.h"
#include "lookup.h"
#include "uai.h"

#include "main.h"

int main() {

    srand(time(NULL));

    settings.init();

    genLookupTables();

    uai();

    return 0;
}
