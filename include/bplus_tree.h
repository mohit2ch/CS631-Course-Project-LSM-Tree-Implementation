
#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <optional>
#include <cstddef>

class BPlusTree {
private:
    struct Node {
        bool is_leaf;

        std::vector<std::string> keys;

        // Internal node children
        std::vector<std::unique_ptr<Node>> children;

        // Leaf node values
        std::vector<std::string> values;

        // Non-owning pointer to next leaf
        Node* next = nullptr;

        explicit Node(bool leaf)
            : is_leaf(leaf) {}
    };

    std::unique_ptr<Node> root;

    size_t max_keys;

    Node* find_leaf(const std::string& key) const;

    struct InsertResult {
        std::string separator;
        std::unique_ptr<Node> right_node;
    };

    InsertResult insert_recursive(
        Node* node,
        const std::string& key,
        const std::string& value
    );

size_t entry_count = 0;

public:
    explicit BPlusTree(size_t max_keys = 3);

    void insert(
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

    size_t size() const;

    bool validate() const;
};

#endif
