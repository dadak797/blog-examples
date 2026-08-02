#include "prime_number.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

namespace {

const int LAST_NUMBER = 1000000;
const int WARMUP_RUNS = 2;
const int TIMED_RUNS = 30;

double ElapsedMs(std::chrono::steady_clock::time_point start,
                  std::chrono::steady_clock::time_point end) {
  return std::chrono::duration<double, std::milli>(end - start).count();
}

struct Stats {
  double mean;
  double median;
  double stddev;
  double min;
};

Stats ComputeStats(std::vector<double> times) {
  const size_t n = times.size();
  const double mean = std::accumulate(times.begin(), times.end(), 0.0) / n;

  std::sort(times.begin(), times.end());
  const double median = (n % 2 == 0)
      ? (times[n / 2 - 1] + times[n / 2]) / 2.0
      : times[n / 2];

  double variance = 0.0;
  for (double t : times) variance += (t - mean) * (t - mean);
  variance /= (n - 1);

  return { mean, median, std::sqrt(variance), times.front() };
}

}  // namespace

int main() {
  std::vector<double> times;

  for (int i = 0; i < WARMUP_RUNS; ++i) {
    auto start = std::chrono::steady_clock::now();
    int primeCount = CountPrimes(LAST_NUMBER);
    auto end = std::chrono::steady_clock::now();
    std::cout << "[Native warmup " << i << "] " << ElapsedMs(start, end)
               << " ms, primes=" << primeCount << std::endl;
  }

  for (int i = 0; i < TIMED_RUNS; ++i) {
    auto start = std::chrono::steady_clock::now();
    int primeCount = CountPrimes(LAST_NUMBER);
    auto end = std::chrono::steady_clock::now();
    double elapsed = ElapsedMs(start, end);
    times.push_back(elapsed);
    std::cout << "[Native run " << i << "] " << elapsed
               << " ms, primes=" << primeCount << std::endl;
  }

  Stats s = ComputeStats(times);
  std::cout << "--- Native stats (n=" << TIMED_RUNS << ") ---" << std::endl;
  std::cout << "mean=" << s.mean << " ms, median=" << s.median
             << " ms, stddev=" << s.stddev << " ms, min=" << s.min << " ms"
             << std::endl;

  return 0;
}
