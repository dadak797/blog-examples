function computeStats(times) {
  if (times.length < 2) {
    throw new Error("At least two timed runs are required.");
  }

  const n = times.length;

  // Mean
  let sum = 0;
  for (const time of times) {
    sum += time;
  }
  const mean = sum / n;

  // Variance
  let squaredDiffSum = 0;
  for (const time of times) {
    const diff = time - mean;
    squaredDiffSum += diff * diff;
  }
  const variance = squaredDiffSum / n;
  const stddev = Math.sqrt(variance);

  return { mean, stddev };
}

function runBenchmark(label, warmupRuns, timedRuns, expectedResult, fn) {
  const warmupTimes = [];
  for (let i = 0; i < warmupRuns; ++i) {
    const start = performance.now();
    const result = fn();
    const end = performance.now();
    if (result !== expectedResult) {
      throw new Error(
        `${label} returned ${result}, expected ${expectedResult}`,
      );
    }
    const elapsed = end - start;
    warmupTimes.push(elapsed);
  }

  const times = [];
  for (let i = 0; i < timedRuns; ++i) {
    const start = performance.now();
    const result = fn();
    const end = performance.now();
    if (result !== expectedResult) {
      throw new Error(
        `${label} returned ${result}, expected ${expectedResult}`,
      );
    }
    const elapsed = end - start;
    times.push(elapsed);
  }

  return { label, warmupTimes, times };
}

function waitForPaint() {
  return new Promise((resolve) => {
    requestAnimationFrame(() => setTimeout(resolve, 0));
  });
}
