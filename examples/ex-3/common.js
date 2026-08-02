function computeStats(times) {
  const n = times.length;
  const mean = times.reduce((a, b) => a + b, 0) / n;

  const sorted = [...times].sort((a, b) => a - b);
  const mid = Math.floor(n / 2);
  const median =
    n % 2 === 0 ? (sorted[mid - 1] + sorted[mid]) / 2 : sorted[mid];

  const variance = times.reduce((a, b) => a + (b - mean) ** 2, 0) / (n - 1);
  const stddev = Math.sqrt(variance);

  return { mean, median, stddev, min: sorted[0] };
}

function runBenchmark(label, warmupRuns, timedRuns, fn) {
  console.log(`\n=== ${label} ===`);

  for (let i = 0; i < warmupRuns; ++i) {
    const start = performance.now();
    const result = fn();
    const end = performance.now();
    console.log(
      `[${label} warmup ${i}] ${(end - start).toFixed(3)} ms, primes=${result}`,
    );
  }

  const times = [];
  for (let i = 0; i < timedRuns; ++i) {
    const start = performance.now();
    const result = fn();
    const end = performance.now();
    const elapsed = end - start;
    times.push(elapsed);
    console.log(
      `[${label} run ${i}] ${elapsed.toFixed(3)} ms, primes=${result}`,
    );
  }

  const { mean, median, stddev, min } = computeStats(times);
  console.log(`--- ${label} stats (n=${timedRuns}) ---`);
  console.log(
    `mean=${mean.toFixed(3)} ms, median=${median.toFixed(3)} ms, ` +
      `stddev=${stddev.toFixed(3)} ms, min=${min.toFixed(3)} ms`,
  );

  return { label, mean, median, stddev, min };
}
