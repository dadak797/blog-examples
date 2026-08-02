## Build Wasm

```
em++ prime_number.cpp -O3 -o wasm_prime_number.js -s EXPORTED_FUNCTIONS='["_CountPrimes"]'
```

## Run JavaScript and Wasm

```
const LAST_NUMBER = 1000000;
const WARMUP_RUNS = 2;
const TIMED_RUNS = 30;
const jsResults = runBenchmark("JS", WARMUP_RUNS, TIMED_RUNS, () => countPrimes(LAST_NUMBER));
const wasmResults = runBenchmark("WASM", WARMUP_RUNS, TIMED_RUNS, () => Module._CountPrimes(LAST_NUMBER));
```

## Build Native

```
g++ -O3 -std=c++17 main.cpp prime_number.cpp -o native_prime
```
