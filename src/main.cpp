#include <sstream>
#include <string>

#include "board.hpp"
#include "lookup.hpp"
#include "uai.hpp"
#include "tuner/tuner.hpp"

#include "main.hpp"

int main() {
    srand(time(NULL));

    #if TUNING
        tuner.run();
    #else
        uai();
    #endif

    return 0;
}
