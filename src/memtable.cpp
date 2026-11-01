#include "memtable.h"

void Memtable::put(
    const std::string& key,
    const std::string& value
) {
    data[key] = value;
}

std::optional<std::string> Memtable::get(
    const std::string& key
) const {
    auto it = data.find(key);

    if (it == data.end()) {
        return std::nullopt;
    }

    return it->second;
}

size_t Memtable::size() const {
    return data.size();
}

bool Memtable::empty() const {
    return data.empty();
}

std::vector<std::pair<std::string, std::string>>
Memtable::get_all() const {
    return {
        data.begin(),
        data.end()
    };
}