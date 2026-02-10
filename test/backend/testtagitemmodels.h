/**
 * Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include <QTest>

#include "models/flatteningproxymodel.h"
#include "models/tagitemmodel.h"
#include "persistedtreeitemmodelstestbase.h"

class TestTagItemModels : public PersistedTreeItemModelsTestBase
{
    Q_OBJECT

public:
    explicit TestTagItemModels(QObject *parent = nullptr);

private:
    std::unique_ptr<TagItemModel> model;
    std::unique_ptr<FlatteningProxyModel> flat_model;

    // Helper functions:
    void assert_initial_dataset_representation_base_model() const;
    void assert_initial_dataset_representation_flat_model() const;
    void assert_correct_from_source_mapping_recursively(
        const QModelIndex& source_index = QModelIndex(),
        int expected_proxy_row = 0
    ) const;
    void assert_correct_proxy_mapping() const;
    void assert_correctness_of_proxy_models() const;
    void assert_model_persistence() const;
    static void remove_single_row_without_children(QAbstractItemModel& model);
    static void remove_children_of_first_top_level_index(QAbstractItemModel& model);


private slots:
    // Test setup/cleanup:
    void initTestCase();
    void init();
    void cleanup();

    // Test functions:
    void test_initial_dataset_represented_correctly() const;
    void test_remove_single_row() const;
    void test_remove_rows_with_children() const;
    void test_remove_single_row_with_nested_children() const;
    void test_create_toplevel_tag() const;
    void test_create_tag_with_parent() const;
    void test_data_change_of_toplevel_item() const;
    void test_data_change_of_child_item() const;
    void test_changing_parent_new_parent_can_not_be_descendant() const;
    void test_changing_parent_new_parent_can_not_be_oneself() const;
    void test_removing_parent() const;
    void test_changing_parent_to_grand_parent() const;
    void test_changing_parent_to_different_subtree() const;
};
