#include "lsm_tree.h"

#include <cassert>
#include <iostream>

int main() {
    LSMTree lsm(3);

    lsm.put("apple", "10");
    lsm.put("banana", "20");
    lsm.put("cherry", "30");

    // First SSTable should have been created.
    assert(lsm.get("apple").value() == "10");

    // Create another SSTable.
    lsm.put("date", "40");
    lsm.put("elderberry", "50");
    lsm.put("fig", "60");

    assert(lsm.get("date").value() == "40");

    // Update an existing key.
    lsm.put("apple", "100");

    assert(lsm.get("apple").value() == "100");

    // Missing key.
    assert(!lsm.get("grape").has_value());

    std::cout << "All LSMTree tests passed!\n";

    return 0;
}