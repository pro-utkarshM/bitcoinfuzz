## Running the Competency Test

Below is a step-by-step guide to build and execute the entire competency-test suite on a Linux machine.

---

### 1. Prerequisites

Install all required system packages:

```bash
sudo apt-get update
sudo apt-get install -y \
    clang libclang-dev \       # for libFuzzer instrumentation  ([libFuzzer – a library for coverage-guided fuzz testing. - LLVM](https://llvm.org/docs/LibFuzzer.html?utm_source=chatgpt.com))  
    cmake make \               # to configure/build C++ harness with CMake  ([Fuzzing LLVM libraries and tools — LLVM 21.0.0git documentation](https://llvm.org/docs/FuzzingLLVM.html?utm_source=chatgpt.com))  
    libbitcoinconsensus-dev \  # Bitcoin Core consensus library  
    cargo                      # to build Rust staticlib  
```

---

### 2. Clone & Enter the Test Directory

```bash
git clone https://github.com/<your-username>/bitcoinfuzz.git
cd bitcoinfuzz/competency-test
```

---

### 3. Verify Seed Corpus

Ensure `seeds/` contains:

- `genesis_block.bin` – the mainnet genesis block blob  
- `witness_zero.blob` – a 1-TX block with zero witness count  

These serve as the initial seed corpus for libFuzzer  ([libFuzzer – a library for coverage-guided fuzz testing. - LLVM](https://llvm.org/docs/LibFuzzer.html?utm_source=chatgpt.com)).

---

### 4. Build & Run All Targets

Make the build script executable and run it:

```bash
chmod +x scripts/build_and_run.sh
bash scripts/build_and_run.sh
```

What happens:

1. **C++ harness** (`modules/cpp_block`) is built out-of-source with CMake, linking libbitcoinconsensus and `librustblock.a`, and compiled with AddressSanitizer & libFuzzer flags  ([fuzzing/tutorial/libFuzzerTutorial.md at master · google/fuzzing - GitHub](https://github.com/google/fuzzing/blob/master/tutorial/libFuzzerTutorial.md?utm_source=chatgpt.com)).  
2. **Rust crate** (`modules/rust_block`) is compiled via `cargo build` into `target/debug/librustblock.a`.  
3. The **fuzzer** runs `./modules/cpp_block/build/fuzz_block` against `seeds/` for 60 seconds, writing any crash inputs to `modules/cpp_block/build/crashes/`.

---

### 5. Examining Crash Artifacts

If the harness detects a mismatch, libFuzzer will abort and write crash files (`id_*.bin`) in:

```
modules/cpp_block/build/crashes/
```

To reproduce and minimize a crash:

```bash
# Re-run the fuzzer on a single crash input
./modules/cpp_block/build/fuzz_block \
  -runs=1 \
  modules/cpp_block/build/crashes/id_000000XX.bin
```

Use the resulting input to draft your preliminary bug report  ([fuzzing/docs/good-fuzz-target.md at master · google/fuzzing - GitHub](https://github.com/google/fuzzing/blob/master/docs/good-fuzz-target.md?utm_source=chatgpt.com)).

---
