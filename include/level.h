#pragma once

#include "bplus_tree.h"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Level {
private:
    size_t level_id;
    size_t capacity_limit;
    BPlusTree tree;

public:
    Level(
        size_t level_id,
        size_t capacity_limit,
        size_t bplus_max_keys = 32
    );

    void put(
        const std::string& key,
        const std::string& value
    );

    std::optional<std::string> get(
        const std::string& key
    ) const;

    std::vector<std::pair<std::string, std::string>>
    range_scan(
        const std::string& start,
        const std::string& end
    ) const;

    std::vector<std::pair<std::string, std::string>>
    entries() const;

    size_t id() const;
    size_t size() const;
    size_t capacity() const;

    bool is_full() const;
    bool empty() const;

    std::optional<std::string> min_key() const;
    std::optional<std::string> max_key() const;

    bool validate() const;
};