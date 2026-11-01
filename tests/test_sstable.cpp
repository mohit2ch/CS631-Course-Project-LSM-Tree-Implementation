
#include "memtable.h"
#include "sstable.h"

#include <cassert>
#include <iostream>

int main() {
    Memtable mem;

    mem.put("cherry", "30");
    mem.put("apple", "10");
    mem.put("banana", "20");

    // Export sorted entries.
    auto entries = mem.get_all();

    // Create SSTable.
    assert(SSTable::create("test.sst", entries));

    // Open SSTable independently.
    SSTable table;

    assert(table.open("test.sst"));

    // Verify entry count.
    assert(table.size() == 3);

    // Verify lookups.
    assert(table.get("apple").value() == "10");
    assert(table.get("banana").value() == "20");
    assert(table.get("cherry").value() == "30");

    // Missing key.
    assert(!table.get("grape").has_value());

    // Verify empty values are distinguishable from missing keys.
    mem.put("empty", "");
    assert(mem.get("empty").has_value());

    std::cout << "All SSTable tests passed!\n";

    return 0;
}
