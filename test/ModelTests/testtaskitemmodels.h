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

#ifndef TESTTASKITEMMODEL_H
#define TESTTASKITEMMODEL_H

#include <memory>

#include <QObject>
#include <QTest>

#include "../../src/app/model/task.h"
#include "../../src/app/model/taskitemmodel.h"
#include "persistedtreeitemmodelstestbase.h"

class TestTaskItemModel : public PersistedTreeItemModelsTestBase
{
    Q_OBJECT

private:
    std::unique_ptr<TaskItemModel> model;

    // Helper functions:
    void assert_initial_dataset_representation_base_model();
    void assert_model_persistence();
    static void assert_index_equality(
        const QModelIndex& index1,
        const QModelIndex& index2
    );
    void find_task_by_title_and_assert_correctness_of_data(
        const QString& title,
        Task::Status status,
        const QDateTime& start_datetime,
        const QDateTime& due_datetime,
        int number_of_children
    );

public:
    explicit TestTaskItemModel(QObject *parent = nullptr);

private slots:
    // Test setup/cleanup:
    void initTestCase();
    void init();
    void cleanup();

    // Test functions:
    void test_initial_dataset_represented_correctly();
    void test_model_stores_text_documents();
    void test_data_change_of_unique_task();
    void test_data_change_of_cloned_task();
    void test_remove_rows();
    void test_create_task();
    void test_add_dependency();
    void test_can_not_create_dependency_cycle();

};

#endif // TESTTASKITEMMODEL_H
