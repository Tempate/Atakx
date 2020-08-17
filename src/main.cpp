#include <sstream>
#include <string>

#include "board.hpp"
#include "lookup.hpp"
#include "uai.hpp"

#include "tuners/genetic/main.hpp"
#include "main.hpp"

int main() {
    srand(time(NULL));

    #if TUNING
        genetic_tuner.run();
    #else
        uai();
    #endif

    return 0;
}
