'use strict';

const createModule = require('./prime_number.js');

const LAST_NUMBER = 1000000;
const WARMUP_RUNS = 2;
const TIMED_RUNS = 30;

function countPrimes(limit) {
  if (limit < 2) return 0;
  let count = 0;
  for (let i = 2; i <= limit; ++i) {
    let isPrime = true;
    for (let j = 2; j * j <= i; ++j) {
      if (i % j === 0) {
        isPrime = false;
        break;
      }
    }
    if (isPrime) ++count;
  }
  return count;
}

function computeStats(times) {
  const n = times.length;
  const mean = times.reduce((a, b) => a + b, 0) / n;

  const sorted = [...times].sort((a, b) => a - b);
  const mid = Math.floor(n / 2);
  const median = n % 2 === 0 ? (sorted[mid - 1] + sorted[mid]) / 2 : sorted[mid];

  const variance = times.reduce((a, b) => a + (b - mean) ** 2, 0) / (n - 1);
  const stddev = Math.sqrt(variance);

  return { mean, median, stddev, min: sorted[0] };
}

function runBenchmark(label, fn) {
  console.log(`\n=== ${label} ===`);

  for (let i = 0; i < WARMUP_RUNS; ++i) {
    const start = performance.now();
    const result = fn();
    const end = performance.now();
    console.log(`[${label} warmup ${i}] ${(end - start).toFixed(3)} ms, primes=${result}`);
  }

  const times = [];
  for (let i = 0; i < TIMED_RUNS; ++i) {
    const start = performance.now();
    const result = fn();
    const end = performance.now();
    const elapsed = end - start;
    times.push(elapsed);
    console.log(`[${label} run ${i}] ${elapsed.toFixed(3)} ms, primes=${result}`);
  }

  const { mean, median, stddev, min } = computeStats(times);
  console.log(`--- ${label} stats (n=${TIMED_RUNS}) ---`);
  console.log(
    `mean=${mean.toFixed(3)} ms, median=${median.toFixed(3)} ms, ` +
    `stddev=${stddev.toFixed(3)} ms, min=${min.toFixed(3)} ms`,
  );

  return { label, mean, median, stddev, min };
}

(async () => {
  const Module = await createModule();

  const results = [
    runBenchmark('JS', () => countPrimes(LAST_NUMBER)),
    runBenchmark('WASM', () => Module._CountPrimes(LAST_NUMBER)),
  ];

  console.log('\n=== Summary ===');
  for (const r of results) {
    console.log(
      `${r.label}: mean=${r.mean.toFixed(3)} ms, median=${r.median.toFixed(3)} ms, ` +
      `stddev=${r.stddev.toFixed(3)} ms, min=${r.min.toFixed(3)} ms`,
    );
  }
})();
