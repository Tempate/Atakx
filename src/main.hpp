#ifndef MAIN_HPP_
#define MAIN_HPP_

#define ENGINE_NAME "Funes"
#define ENGINE_AUTHOR "tempate"

#include <array>
#include <cassert>
#include <iostream>
#include <vector>

#define DEBUG false
#define TUNING false
#define THREADS 1

enum { RANDOM_PLAYER, MOST_CAPTURES, ALPHABETA };

#define TYPE ALPHABETA

#define FILES 7
#define RANKS 7

#define MATE_SCORE 100000

enum { BLACK, WHITE };

#endif // #ifndef MAIN_HPP_