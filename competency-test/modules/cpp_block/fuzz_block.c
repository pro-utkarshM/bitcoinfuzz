#include <cstdint>
#include <vector>
#include <fuzzer/FuzzedDataProvider.h>
#include "primitives/block.h"            // Bitcoin Core headers
#include "serialize.h"

extern "C" {
    // C ABI function from Rust crate
    bool rust_block_deserialize(const uint8_t* data, size_t len,
                                int32_t* version, size_t* tx_count);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    // Copy input into a vector
    std::vector<uint8_t> blob(data, data + size);

    // --- Bitcoin Core deserialization ---
    CBlock core_block;
    try {
        CDataStream ds(blob, SER_NETWORK, PROTOCOL_VERSION);
        ds >> core_block;
    } catch (...) {
        return 0; // Skip invalid inputs
    }
    int32_t core_version = core_block.nVersion;
    size_t core_txcount = core_block.vtx.size();

    // --- rust-bitcoin deserialization via C ABI ---
    int32_t rust_version = 0;
    size_t rust_txcount = 0;
    bool ok = rust_block_deserialize(blob.data(), blob.size(),
                                     &rust_version, &rust_txcount);
    if (!ok) {
        return 0; // rust-bitcoin rejects input
    }

    // --- Differential check ---
    if (core_version != rust_version || core_txcount != rust_txcount) {
        // Discrepancy found → crash to report
        abort();
    }
    return 0;
}