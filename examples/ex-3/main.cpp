#include "prime_number.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

const int LAST_NUMBER = 1000000;
const int PRIME_COUNT = 78498;
const int WARMUP_RUNS = 2;
const int TIMED_RUNS = 30;

double ElapsedMs(std::chrono::steady_clock::time_point start,
                  std::chrono::steady_clock::time_point end) {
  return std::chrono::duration<double, std::milli>(end - start).count();
}

struct Stats {
  double mean;
  double stddev;
};

Stats ComputeStats(const std::vector<double>& times) {
  if (times.size() < 2) {
    throw std::invalid_argument("At least two timed runs are required.");
  }

  const size_t n = times.size();

  // Mean
  double sum = 0.0;
  for (const double time : times) {
    sum += time;
  }
  const double mean = sum / n;

  // Variance
  double squaredDiffSum = 0.0;
  for (const double time : times) {
    const double diff = time - mean;
    squaredDiffSum += diff * diff;
  }
  const double variance = squaredDiffSum / n;
  const double stddev = std::sqrt(variance);

  return { mean, stddev };
}

}  // namespace

int main() {
  std::vector<double> warmupTimes;
  warmupTimes.reserve(WARMUP_RUNS);
  std::vector<double> times;
  times.reserve(TIMED_RUNS);

  for (int i = 0; i < WARMUP_RUNS; ++i) {
    const auto start = std::chrono::steady_clock::now();
    const int result = CountPrimes(LAST_NUMBER);
    const auto end = std::chrono::steady_clock::now();
    if (result != PRIME_COUNT) {
      throw std::runtime_error(
          "Native returned an unexpected result during warmup.");
    }
    const double elapsed = ElapsedMs(start, end);
    warmupTimes.push_back(elapsed);
  }

  for (int i = 0; i < TIMED_RUNS; ++i) {
    const auto start = std::chrono::steady_clock::now();
    const int result = CountPrimes(LAST_NUMBER);
    const auto end = std::chrono::steady_clock::now();
    if (result != PRIME_COUNT) {
      throw std::runtime_error(
          "Native returned an unexpected result during timed run.");
    }
    const double elapsed = ElapsedMs(start, end);
    times.push_back(elapsed);
  }

  Stats s = ComputeStats(times);
  std::cout << "--- Native stats (n=" << TIMED_RUNS << ") ---" << std::endl;
  std::cout << "mean=" << s.mean << " ms, stddev=" << s.stddev << " ms"
            << std::endl;

  return 0;
}
