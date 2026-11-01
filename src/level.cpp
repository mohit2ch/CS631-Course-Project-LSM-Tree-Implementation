#include "level.h"

#include <stdexcept>

Level::Level(
    size_t level_id,
    size_t capacity_limit,
    size_t bplus_max_keys
)
    : level_id(level_id),
      capacity_limit(capacity_limit),
      tree(bplus_max_keys) {

    if (capacity_limit == 0) {
        throw std::invalid_argument(
            "Level capacity must be greater than zero"
        );
    }
}

void Level::put(
    const std::string& key,
    const std::string& value
) {
    tree.insert(key, value);
}

std::optional<std::string> Level::get(
    const std::string& key
) const {
    return tree.get(key);
}

std::vector<std::pair<std::string, std::string>>
Level::range_scan(
    const std::string& start,
    const std::string& end
) const {
    return tree.range_scan(start, end);
}

std::vector<std::pair<std::string, std::string>>
Level::entries() const {
    return tree.range_scan("", "\x7f");
}

size_t Level::id() const {
    return level_id;
}

size_t Level::size() const {
    return tree.size();
}

size_t Level::capacity() const {
    return capacity_limit;
}

bool Level::is_full() const {
    return size() >= capacity_limit;
}

bool Level::empty() const {
    return size() == 0;
}

std::optional<std::string> Level::min_key() const {
    auto result = tree.range_scan("", "\x7f");

    if (result.empty()) {
        return std::nullopt;
    }

    return result.front().first;
}

std::optional<std::string> Level::max_key() const {
    auto result = tree.range_scan("", "\x7f");

    if (result.empty()) {
        return std::nullopt;
    }

    return result.back().first;
}

bool Level::validate() const {
    return tree.validate();
}