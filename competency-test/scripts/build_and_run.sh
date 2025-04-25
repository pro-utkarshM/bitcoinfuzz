#!/bin/bash
set -e

# Build C++ fuzz target
pushd modules/cpp_block
mkdir -p build && cd build
cmake ..
make -j$(nproc) fuzz_block
popd

# Build Rust static lib
pushd modules/rust_block
cargo build
popd

# (Go target build is similar...)

# Run the C++ fuzzer
pushd modules/cpp_block/build
mkdir -p crashes
./fuzz_block \
  -artifact_prefix=crashes/ \
  -max_total_time=60 \
  ../../../seeds/
popd