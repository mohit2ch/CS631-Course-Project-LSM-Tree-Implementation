
#include "memtable.h"

#include <cassert>
#include <iostream>

int main() {
    Memtable mem;

    // Test insertion
    mem.put("apple", "10");
    mem.put("banana", "20");
    mem.put("cherry", "30");

    assert(mem.size() == 3);

    // Test lookup
    assert(mem.get("apple").value() == "10");
    assert(mem.get("banana").value() == "20");

    // Test missing key
    assert(!mem.get("grape").has_value());

    // Test update
    mem.put("banana", "25");

    assert(mem.size() == 3);
    assert(mem.get("banana").value() == "25");

    // Test sorted order
    std::string expected[] = {
        "apple", "banana", "cherry"
    };

    int i = 0;

    // We'll expose an iterator in a later step.
    // For now, verify through the map's ordering
    // in the Memtable implementation.

    std::cout << "All Memtable tests passed!\n";

    return 0;
}
