
#ifndef SSTABLE_H
#define SSTABLE_H

#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <cstdint>

class SSTable {
public:
    using Entry = std::pair<std::string, std::string>;

    // Create a new immutable SSTable.
    static bool create(
        const std::string& filename,
        const std::vector<Entry>& entries
    );

    // Open an existing SSTable.
    bool open(const std::string& filename);

    // Retrieve a value.
    std::optional<std::string> get(
        const std::string& key
    ) const;

    size_t size() const;

private:
    std::string filename;
    std::vector<Entry> entries;
};

#endif
