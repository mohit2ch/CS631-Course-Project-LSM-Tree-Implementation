
cmake_minimum_required(VERSION 3.16)

project(LSMPostgres LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lsm
    src/memtable.cpp
    src/sstable.cpp
    src/lsm_tree.cpp
    src/bplus_tree.cpp
    src/level.cpp
)

target_include_directories(lsm PUBLIC include)

add_executable(test_memtable
    tests/test_memtable.cpp
)

target_link_libraries(test_memtable PRIVATE lsm)

enable_testing()
add_test(NAME MemtableTests COMMAND test_memtable)

add_executable(test_sstable
    tests/test_sstable.cpp
)

target_link_libraries(test_sstable PRIVATE lsm)

enable_testing()
add_test(NAME SSTableTests COMMAND test_sstable)

add_executable(test_lsm_tree
    tests/test_lsm_tree.cpp
)

target_link_libraries(test_lsm_tree PRIVATE lsm)

enable_testing()
add_test(NAME LSMTreeTests COMMAND test_lsm_tree)

add_executable(test_bplus_tree
    tests/test_bplus_tree.cpp
)

target_link_libraries(test_bplus_tree PRIVATE lsm)

enable_testing()
add_test(NAME BPLUSTreeTests COMMAND test_bplus_tree)

add_executable(test_bplus_tree_random
    tests/test_bplus_tree_random.cpp
)

target_link_libraries(test_bplus_tree_random PRIVATE lsm)

enable_testing()
add_test(NAME BPLUSTreeRandomTests COMMAND test_bplus_tree_random)


add_executable(test_level
    tests/test_level.cpp
)

target_link_libraries(test_level PRIVATE lsm)

enable_testing()
add_test(NAME LevelTests COMMAND test_level)