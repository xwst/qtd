/**
 * Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
 *
 * This file is part of qtd.
 *
 * qtd is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * qtd is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * qtd. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TREENODE_H
#define TREENODE_H

#include <memory>
#include <vector>

#include <QVariant>

#include "uniquedataitem.h"

class TreeNode {

private:
    std::shared_ptr<UniqueDataItem> data;
    const TreeNode* parent;
    std::vector<std::unique_ptr<TreeNode>> children;

    TreeNode(std::shared_ptr<UniqueDataItem> data, const TreeNode* parent);
    static std::unique_ptr<TreeNode> create(
        std::shared_ptr<UniqueDataItem> data,
        const TreeNode* parent
    );

public:
    static std::unique_ptr<TreeNode> create(
        std::unique_ptr<UniqueDataItem> data,
        const TreeNode* parent = nullptr
    );
    static std::unique_ptr<TreeNode> clone(
        const TreeNode* to_be_cloned,
        const TreeNode* parent = nullptr
    );

    [[nodiscard]] const TreeNode* get_parent() const;
    [[nodiscard]] TreeNode* get_child(int row) const;
    void add_child(std::unique_ptr<UniqueDataItem> child_data);
    void add_child(std::unique_ptr<TreeNode> new_child);
    [[nodiscard]] int get_child_count() const;

    void remove_children(int row, int count);
    [[nodiscard]] int get_row_in_parent() const;

    [[nodiscard]] QVariant get_data(int role) const;
    void set_data(const QVariant& value, int role);
};

#endif // TREENODE_H
