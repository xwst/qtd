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

#ifndef TESTTAGITEMMODELS_H
#define TESTTAGITEMMODELS_H

#include <memory>

#include <QTest>

#include "../../src/app/model/flatteningproxymodel.h"
#include "../../src/app/model/tagitemmodel.h"
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
    void assert_initial_dataset_representation_base_model();
    void assert_initial_dataset_representation_flat_model();
    void assert_correct_from_source_mapping_recursively(
        const QModelIndex& source_index = QModelIndex(),
        int expected_proxy_row = 0
    );
    void assert_correct_proxy_mapping();
    void assert_correctness_of_proxy_models();
    void assert_model_persistence();
    static void remove_single_row_without_children(QAbstractItemModel& model);
    static void remove_children_of_first_top_level_index(QAbstractItemModel& model);


private slots:
    // Test setup/cleanup:
    void initTestCase();
    void init();
    void cleanup();

    // Test functions:
    void test_initial_dataset_represented_correctly();
    void test_remove_single_row();
    void test_remove_rows_with_children();
    void test_remove_single_row_with_nested_children();
    void test_create_toplevel_tag();
    void test_create_tag_with_parent();
    void test_data_change_of_toplevel_item();
    void test_data_change_of_child_item();
};

#endif // TESTTAGITEMMODELST_H
