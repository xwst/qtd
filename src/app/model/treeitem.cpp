#include "treeitem.h"

TreeItem::TreeItem(std::shared_ptr<UniqueDataItem> data, TreeItem* parent)
    : data(data), parent(parent)
{
    this->children = std::make_shared<std::deque<std::unique_ptr<TreeItem>>>();
}

/**
 * @brief Create a new TreeItem with the given data item.
 *
 * Use this function, if no other TreeItem with the given data item exists.
 * @sa TreeItem::create_mirrored
 */
std::unique_ptr<TreeItem> TreeItem::create(
    std::unique_ptr<UniqueDataItem>&& data,
    TreeItem* parent
) {
    return std::unique_ptr<TreeItem>(new TreeItem(std::move(data), parent));
}

/**
 * @brief Mirrors an existing TreeItem but uses a different parent.
 *
 * Use this function, if the new and the provided TreeItem should be identical
 * except for the parent TreeItem. This can be used, to allow a TreeItem to
 * appear multiple times in the tree hierarchy.
 * @sa TreeItem::create
 */
std::unique_ptr<TreeItem> TreeItem::create_mirrored(
    TreeItem* to_be_mirrored,
    TreeItem* parent
) {
    auto result = std::unique_ptr<TreeItem>(new TreeItem(to_be_mirrored->data, parent));
    result->children = to_be_mirrored->children;
    return result;
}

TreeItem* TreeItem::get_parent() const {
    return this->parent;
}

TreeItem* TreeItem::get_child(int row) const {
    return this->children->at(row).get();
}

void TreeItem::add_child(std::unique_ptr<UniqueDataItem>&& child_data) {
    auto new_child = TreeItem::create(std::move(child_data), this);
    this->children->push_back(std::move(new_child));
}

void TreeItem::add_child(std::unique_ptr<TreeItem>&& new_child) {
    new_child->parent = this;
    this->children->push_back(std::move(new_child));
}

int TreeItem::get_child_count() const {
    return static_cast<int>(this->children->size());
}

void TreeItem::remove_children(int row, int count) {
    auto it_first = this->children->begin() + row;
    this->children->erase(it_first, it_first + count);
}

int TreeItem::get_row_in_parent() const {
    int result = 0;
    for (const auto& child : *(this->parent->children))
        if (child.get() == this) return result;
        else result++;
    return -1; // Should not happen
}

QVariant TreeItem::get_data(int role) const {
    return this->data->get_data(role);
}

void TreeItem::set_data(const QVariant& value, int role) {
    this->data->set_data(value, role);
}

