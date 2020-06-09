#include <sstream>
#include <string>

#include "board.h"
#include "lookup.h"
#include "uai.h"
#include "tuner/tuner.h"

#include "main.h"

int main() {

    srand(time(NULL));

    settings.init();

    genLookupTables();

    #if TUNING
        tuner.run();
    #else
        uai();
    #endif

    return 0;
}
