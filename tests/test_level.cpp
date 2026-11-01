#include "level.h"

#include <cassert>
#include <iostream>

int main() {
    Level level(0, 5, 3);

    assert(level.empty());
    assert(level.size() == 0);
    assert(!level.is_full());

    level.put("apple", "red");
    level.put("banana", "yellow");
    level.put("cherry", "red");

    assert(level.size() == 3);
    assert(!level.is_full());

    level.put("date", "brown");
    level.put("elderberry", "purple");

    assert(level.size() == 5);
    assert(level.is_full());

    auto value = level.get("banana");

    assert(value.has_value());
    assert(*value == "yellow");

    auto missing = level.get("grape");
    assert(!missing.has_value());

    // Updating an existing key should not increase size.
    level.put("apple", "green");

    assert(level.size() == 5);
    assert(*level.get("apple") == "green");

    auto range = level.range_scan("banana", "date");

    assert(range.size() == 3);
    assert(range[0].first == "banana");
    assert(range[1].first == "cherry");
    assert(range[2].first == "date");

    assert(level.min_key() == "apple");
    assert(level.max_key() == "elderberry");

    assert(level.validate());

    std::cout << "Level tests passed.\n";
}