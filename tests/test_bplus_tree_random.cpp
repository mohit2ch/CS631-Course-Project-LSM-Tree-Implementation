#include "bplus_tree.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>

std::string make_key(int value) {
    std::ostringstream out;
    out << std::setw(6) << std::setfill('0') << value;
    return out.str();
}

void run_test(size_t max_keys) {
    BPlusTree tree(max_keys);
    std::map<std::string, std::string> reference;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> key_dist(0, 1999);
    std::uniform_int_distribution<int> value_dist(0, 1000000);

    constexpr int NUM_OPERATIONS = 10000;

    for (int i = 0; i < NUM_OPERATIONS; i++) {
        int key_num = key_dist(rng);

        std::string key = make_key(key_num);
        std::string value = "value_" +
                            std::to_string(value_dist(rng));

        // Insert or update in both implementations.
        tree.insert(key, value);
        reference[key] = value;

        // Periodically validate the tree structure.
        if (i % 100 == 0) {
            assert(tree.validate());
        }
    }

    assert(tree.validate());

    // Check the number of unique entries.
    assert(tree.size() == reference.size());

    // Check every key.
    for (const auto& [key, expected_value] : reference) {
        auto actual = tree.get(key);

        assert(actual.has_value());
        assert(*actual == expected_value);
    }

    // Check keys that were never inserted.
    for (int i = 2000; i < 2100; i++) {
        assert(!tree.get(make_key(i)).has_value());
    }

    // Compare randomized range scans.
    for (int i = 0; i < 100; i++) {
        int a = key_dist(rng);
        int b = key_dist(rng);

        if (a > b) {
            std::swap(a, b);
        }

        std::string start = make_key(a);
        std::string end = make_key(b);

        auto actual = tree.range_scan(start, end);

        std::vector<std::pair<std::string, std::string>> expected;

        for (auto it = reference.lower_bound(start);
             it != reference.end() && it->first <= end;
             ++it) {
            expected.push_back(*it);
        }

        assert(actual == expected);
    }

    std::cout << "Passed randomized test with max_keys = "
              << max_keys << ", entries = " << tree.size()
              << '\n';
}

int main() {
    // Exercise different split configurations.
    run_test(2);
    run_test(3);
    run_test(4);
    run_test(7);

    std::cout << "All randomized B+ tree tests passed.\n";
    return 0;
}