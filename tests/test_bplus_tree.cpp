
#include "bplus_tree.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

std::string make_key(int n) {
    std::ostringstream out;
    out << std::setw(3) << std::setfill('0') << n;
    return out.str();
}

int main() {
    BPlusTree tree(3);

    // Test 1: Basic insertion and lookup.
    tree.insert("apple", "10");
    tree.insert("banana", "20");
    tree.insert("cherry", "30");

    assert(tree.get("apple").value() == "10");
    assert(tree.get("banana").value() == "20");
    assert(tree.get("cherry").value() == "30");

    // Test 2: Force leaf and internal node splits.
    for (int i = 0; i < 100; ++i) {
        tree.insert(make_key(i), std::to_string(i));
    }

    assert(tree.size() == 103);

    for (int i = 0; i < 100; ++i) {
        assert(
            tree.get(make_key(i)).value()
            == std::to_string(i)
        );
    }

    // Test 3: Update existing key.
    tree.insert("apple", "999");

    assert(tree.get("apple").value() == "999");
    assert(tree.size() == 103);

    // Test 4: Missing key.
    assert(!tree.get("grape").has_value());

    // Test 5: Range scan.
    auto results = tree.range_scan("010", "020");

    assert(results.size() == 11);

    for (size_t i = 0; i < results.size(); ++i) {
        assert(results[i].first == make_key(10 + i));
    }

    // Test 6: Invalid range.
    assert(tree.range_scan("020", "010").empty());

    std::cout << "All B+ Tree tests passed!\n";

    return 0;
}
