
#include "sstable.h"

#include <fstream>
#include <algorithm>
#include <limits>

bool SSTable::create(
    const std::string& filename,
    const std::vector<Entry>& entries
) {
    std::ofstream out(filename, std::ios::binary);

    if (!out.is_open()) {
        return false;
    }

    // Ensure entries are sorted.
    std::vector<Entry> sorted = entries;

    std::sort(
        sorted.begin(),
        sorted.end(),
        [](const Entry& a, const Entry& b) {
            return a.first < b.first;
        }
    );

    // File header: number of entries.
    uint64_t count = sorted.size();

    out.write(
        reinterpret_cast<const char*>(&count),
        sizeof(count)
    );

    for (const auto& [key, value] : sorted) {
        if (key.size() > std::numeric_limits<uint32_t>::max() ||
            value.size() > std::numeric_limits<uint32_t>::max()) {
            return false;
        }

        uint32_t key_len = key.size();
        uint32_t value_len = value.size();

        out.write(
            reinterpret_cast<const char*>(&key_len),
            sizeof(key_len)
        );

        out.write(
            reinterpret_cast<const char*>(&value_len),
            sizeof(value_len)
        );

        out.write(key.data(), key_len);
        out.write(value.data(), value_len);

        if (!out) {
            return false;
        }
    }

    out.close();

    return out.good();
}

bool SSTable::open(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);

    if (!in.is_open()) {
        return false;
    }

    uint64_t count;

    in.read(
        reinterpret_cast<char*>(&count),
        sizeof(count)
    );

    if (!in || count > 10000000) {
        return false;
    }

    std::vector<Entry> loaded;
    loaded.reserve(static_cast<size_t>(count));

    for (uint64_t i = 0; i < count; i++) {
        uint32_t key_len, value_len;

        in.read(
            reinterpret_cast<char*>(&key_len),
            sizeof(key_len)
        );

        in.read(
            reinterpret_cast<char*>(&value_len),
            sizeof(value_len)
        );

        if (!in) {
            return false;
        }

        // Basic protection against corrupt or unreasonable lengths.
        if (key_len > 16 * 1024 * 1024 ||
            value_len > 64 * 1024 * 1024) {
            return false;
        }

        std::string key(key_len, '\0');
        std::string value(value_len, '\0');

        in.read(key.data(), key_len);
        in.read(value.data(), value_len);

        if (!in) {
            return false;
        }

        // Ensure the file is sorted.
        if (!loaded.empty() &&
            loaded.back().first >= key) {
            return false;
        }

        loaded.emplace_back(
            std::move(key),
            std::move(value)
        );
    }

    // Replace the current state only after successful loading.
    this->filename = filename;
    this->entries = std::move(loaded);

    return true;
}

std::optional<std::string> SSTable::get(
    const std::string& key
) const {
    auto it = std::lower_bound(
        entries.begin(),
        entries.end(),
        key,
        [](const Entry& entry, const std::string& k) {
            return entry.first < k;
        }
    );

    if (it == entries.end() || it->first != key) {
        return std::nullopt;
    }

    return it->second;
}

size_t SSTable::size() const {
    return entries.size();
}
