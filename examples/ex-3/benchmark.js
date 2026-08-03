"use strict";

const LAST_NUMBER = 1000000;
const PRIME_COUNT = 78498;
const WARMUP_RUNS = 2;
const TIMED_RUNS = 30;

const btnBenchmark = document.querySelector("#run-benchmark");
const resultsSection = document.querySelector("#results");
const resultsBody = document.querySelector("#results-body");

let wasmModule;

function formatMilliseconds(value) {
  return `${value.toFixed(3)} ms`;
}

function renderResults(results) {
  resultsBody.replaceChildren();

  for (const result of results) {
    const row = document.createElement("tr");
    const values = [
      result.label,
      formatMilliseconds(result.mean),
      formatMilliseconds(result.stddev),
    ];

    for (const value of values) {
      const cell = document.createElement("td");
      cell.textContent = value;
      row.append(cell);
    }

    resultsBody.append(row);
  }

  resultsSection.hidden = false;
}

async function runBrowserBenchmark() {
  btnBenchmark.disabled = true;
  resultsSection.hidden = true;

  await waitForPaint();

  const jsResult = runBenchmark(
    "JavaScript",
    WARMUP_RUNS,
    TIMED_RUNS,
    PRIME_COUNT,
    () => countPrimes(LAST_NUMBER),
  );
  const wasmResult = runBenchmark(
    "WebAssembly",
    WARMUP_RUNS,
    TIMED_RUNS,
    PRIME_COUNT,
    () => wasmModule._CountPrimes(LAST_NUMBER),
  );
  const jsStat = computeStats(jsResult.times);
  const wasmStat = computeStats(wasmResult.times);
  const results = [
    {
      label: "JavaScript",
      mean: jsStat.mean,
      stddev: jsStat.stddev,
    },
    {
      label: "WebAssembly",
      mean: wasmStat.mean,
      stddev: wasmStat.stddev,
    },
  ];
  renderResults(results);

  btnBenchmark.disabled = false;
}

async function initializeBenchmark() {
  wasmModule = await createPrimeModule();
  btnBenchmark.disabled = false;
}

btnBenchmark.addEventListener("click", runBrowserBenchmark);
initializeBenchmark();
