#ifndef MYRANDOM_H
#define MYRANDOM_H

#include <random>
#include <omp.h>
#include "vec3.h"

std::mt19937_64 rng;
std::uniform_real_distribution<double> unif(0, 1);

struct TMyRand {
  TMyRand() : distribution_(0.0,1.0) {
    int num_threads = std::max(1, omp_get_max_threads());
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
      auto seed = std::random_device()() + thread_id;
      engines_.push_back(std::mt19937_64(seed));
    }
  }

  double operator()() {
    return distribution_(engines_[omp_get_thread_num()]);
  }

  std::vector<std::mt19937_64> engines_;
  std::uniform_real_distribution<double> distribution_;
};

TMyRand MyRand; 

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

#endif