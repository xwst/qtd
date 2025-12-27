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

#pragma once

#include <memory>

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "testmodelwrappers.h"

class TestTreeItemModel : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeItemModel(QObject *parent = nullptr);

private:
    std::unique_ptr<TreeItemModelTestWrapper> model;

    void setup_initial_model();
    void verify_item(
        const QModelIndex& item, const QString& name, int child_count, const QModelIndex& parent
    );
    static QSet<QModelIndex> model_indices_of_row_change_signals(const QSignalSpy& spy);

private slots:
    // Test setup/cleanup:
    void init();
    void cleanup();

    // Tests:
    void test_initial_setup();
    void test_set_data();
    void test_remove_single_row();
    void test_remove_multiple_rows();
    void test_remove_rows_with_children();
    void test_clone_tree_node_clones_children_recursiveley();
    void test_adding_children_to_clones();
    void test_remove_clone();
    void test_remove_child_of_clone();
};
