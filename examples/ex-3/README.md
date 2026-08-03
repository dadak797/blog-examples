# JavaScript, WebAssembly, and Native Benchmark

This example counts the prime numbers up to `1,000,000` using the same
algorithm in JavaScript and C++. The expected result is `78,498`.

Each benchmark performs:

- 2 warm-up runs
- 30 timed runs
- mean and population standard deviation calculations

The browser runs the JavaScript benchmark first and the WebAssembly benchmark
second. The Native benchmark runs separately from the command line.

## Build WebAssembly

Run the following command from the `examples/ex-3` directory:

```sh
em++ prime_number.cpp -O3 -o wasm_prime_number.js \
  -s EXPORTED_FUNCTIONS='["_CountPrimes"]' \
  -s MODULARIZE=1 \
  -s EXPORT_NAME='createPrimeModule'
```

This generates:

- `wasm_prime_number.js`: the `createPrimeModule` factory function
- `wasm_prime_number.wasm`: the compiled WebAssembly module

Calling `createPrimeModule()` returns a Promise that resolves to an initialized
module instance.

## Run the browser benchmark

Serve the `examples/ex-3` directory over HTTP:

```sh
python3 -m http.server 8000
```

Then:

1. Open <http://localhost:8000>.
2. Wait until the **Run Benchmark** button is enabled.
3. Close DevTools before measuring.
4. Select **Run Benchmark**.
5. Check the JavaScript and WebAssembly results in the HTML table.

The table reports the mean and population standard deviation in milliseconds.
Reload the page before each independent benchmark run so that every run starts
with a new JavaScript context and WebAssembly instance.

Do not open `index.html` directly with a `file://` URL because the browser needs
to fetch `wasm_prime_number.wasm` over HTTP.

## Build and run Native with Apple Clang

On macOS, `/usr/bin/g++` is an alias for Apple Clang. Use `clang++` explicitly
when recording an Apple Clang result:

```sh
clang++ -O3 -std=c++17 main.cpp prime_number.cpp -o native_prime
./native_prime
```

The Native benchmark prints only the final statistics after all warm-up and
timed runs have finished.

## Build and run Native with GCC 15

If GCC 15 is installed, use its versioned executable so that the result is not
mistaken for an Apple Clang build:

```sh
g++-15 -O3 -std=c++17 main.cpp prime_number.cpp -o native_prime_gcc
./native_prime_gcc
```

## Comparing results

Record the following information with each result:

- browser and browser version for WebAssembly
- Emscripten version
- Native compiler and compiler version
- CPU and power mode

JavaScript, WebAssembly, and Native use the same prime-counting logic, but their
compiler and runtime optimization pipelines are different. Small differences
should therefore be interpreted as results for the recorded environment rather
than a universal performance ranking.
