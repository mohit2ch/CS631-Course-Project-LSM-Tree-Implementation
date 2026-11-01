#include "lsm_tree.h"

#include <sstream>
#include <stdexcept>

LSMTree::LSMTree(size_t capacity)
    : memtable_capacity(capacity) {}

void LSMTree::put(
    const std::string& key,
    const std::string& value
) {
    memtable.put(key, value);

    if (memtable.size() >= memtable_capacity) {
        flush();
    }
}

void LSMTree::flush() {
    if (memtable.empty()) {
        return;
    }

    std::ostringstream filename;
    filename << "sstable_" << next_sstable_id++ << ".dat";

    std::string path = filename.str();

    auto entries = memtable.get_all();

    if (!SSTable::create(path, entries)) {
        throw std::runtime_error("SSTable creation failed");
    }

    // Newest SSTable comes first.
    sstables.insert(sstables.begin(), path);

    // Clear the memtable after successful flush.
    memtable = Memtable();
}

std::optional<std::string> LSMTree::get(
    const std::string& key
) const {
    auto value = memtable.get(key);

    if (value.has_value()) {
        return value;
    }

    // Search newest SSTables first.
    for (const auto& path : sstables) {
        SSTable table;

        if (!table.open(path)) {
            throw std::runtime_error("Failed to open SSTable");
        }

        value = table.get(key);

        if (value.has_value()) {
            return value;
        }
    }

    return std::nullopt;
}