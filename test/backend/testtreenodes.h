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

#ifndef TESTTREENODES_H
#define TESTTREENODES_H

#include <memory>

#include <QObject>
#include <QTest>

#include "dataitems/treenode.h"

class TestTreeNodes : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeNodes(QObject *parent = nullptr);

private:
    std::unique_ptr<TreeNode> root;

    void setup_dummies();
    static void verify_item(TreeNode* item, const QString& name, int child_count, TreeNode* parent);
    void verify_dummies();


private slots:
    // Test setup/cleanup:
    void init();
    void cleanup();

    // Test functions:
    void test_add_child();
    void test_remove_single_child();
    void test_remove_multiple_children();
    void test_remove_child_hierarchy();
    void test_cloning();
    void test_set_data();
};

#endif // TESTTREENODES_H
