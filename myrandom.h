#ifndef MYRANDOM_H
#define MYRANDOM_H

#include <random>
#include "vec3.h"

std::mt19937_64 rng;
std::uniform_real_distribution<double> unif(0, 1);

//inline double unif(std::mt19937_64 rng) {
//    return rand() / (RAND_MAX + 1.0);
//}

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

#endif