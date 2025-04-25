cmake_minimum_required(VERSION 3.15)
project(cpp_block_fuzz)

find_package(PkgConfig REQUIRED)
pkg_check_modules(BITCOINCONSENSUS REQUIRED libbitcoinconsensus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,fuzzer-no-link -g")

include_directories(${BITCOINCONSENSUS_INCLUDE_DIRS})
link_directories(${BITCOINCONSENSUS_LIBRARY_DIRS})

# Import the Rust static library
add_library(rustblock STATIC IMPORTED)
set_target_properties(rustblock PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../rust_block/target/debug/librustblock.a"
)

# Fuzz target executable
add_executable(fuzz_block fuzz_block.cc)
target_link_libraries(fuzz_block
    ${BITCOINCONSENSUS_LIBRARIES}
    rustblock
    -fsanitize=address
)