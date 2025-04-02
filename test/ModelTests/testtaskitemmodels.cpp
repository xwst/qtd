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

#include "testtaskitemmodels.h"

#include <memory>

#include <QAbstractItemModelTester>
#include <QDateTime>
#include <QObject>
#include <QSet>
#include <QString>
#include <QTest>

#include "../../src/app/model/model_constants.h"
#include "../../src/app/model/task.h"
#include "../../src/app/util.h"
#include "../testhelpers.h"
#include "persistedtreeitemmodelstestbase.h"

TestTaskItemModel::TestTaskItemModel(QObject *parent)
    : PersistedTreeItemModelsTestBase{parent}
{}

void TestTaskItemModel::initTestCase() {
    PersistedTreeItemModelsTestBase::initTestCase();
    TestHelpers::assert_table_exists("tasks");
}

void TestTaskItemModel::init() {
    PersistedTreeItemModelsTestBase::init();
    TestHelpers::setup_item_model(this->model, this->get_db_connection_name());
}

void TestTaskItemModel::cleanup() {
    this->assert_model_persistence();
    this->model.reset();
    PersistedTreeItemModelsTestBase::cleanup();
}

void TestTaskItemModel::test_initial_dataset_represented_correctly() {
    this->assert_initial_dataset_representation_base_model();
}

void TestTaskItemModel::test_model_stores_text_documents() {
    QEXPECT_FAIL("", "Document storage is not yet implemented!", Continue);
    QVERIFY(this->model->data(this->model->index(0, 0), details_role).isValid());
}

void TestTaskItemModel::test_data_change_of_unique_task() {
    const QString title = "Buy groceries";
    const auto test_index = TestHelpers::find_model_index_by_display_role(*this->model, title);
    QVERIFY(test_index.isValid());

    const auto due_time = test_index.data(due_role).toDateTime();
    const auto uuid = test_index.data(uuid_role).toUuid();
    QCOMPARE(test_index.data(active_role), Task::open);

    const auto new_start_time = QDateTime::currentDateTime();
    QVERIFY(this->model->setData(test_index, new_start_time, start_role));
    QVERIFY(this->model->setData(test_index, Task::closed, active_role));

    QCOMPARE(test_index.data(active_role), Task::closed);
    QCOMPARE(test_index.data(start_role).toDateTime(), new_start_time);

    QCOMPARE(test_index.data().toString(), title);
    QCOMPARE(test_index.data(due_role).toDateTime(), due_time);
    QCOMPARE(test_index.data(uuid_role).toUuid(), uuid);
}

void TestTaskItemModel::test_data_change_of_cloned_task() {
    const auto test_index = TestHelpers::find_model_index_by_display_role(
        *this->model,
        "Fix printer",
        TestHelpers::find_model_index_by_display_role(
            *this->model,
            "Buy groceries"
        )
    );
    const auto test_index_clone = TestHelpers::find_model_index_by_display_role(
        *this->model,
        "Fix printer",
        TestHelpers::find_model_index_by_display_role(
            *this->model,
            "Answer landlords mail"
        )
    );

    QVERIFY(test_index.isValid());
    QVERIFY(test_index_clone.isValid());
    QCOMPARE_NE(test_index_clone, test_index);
    TestTaskItemModel::assert_index_equality(test_index, test_index_clone);

    const QString new_title = "Fix printer and refill ink";
    QVERIFY(this->model->setData(test_index, new_title, Qt::DisplayRole));
    QCOMPARE(test_index_clone.data().toString(), new_title);
    TestTaskItemModel::assert_index_equality(test_index, test_index_clone);

    const auto new_status = (test_index_clone.data(active_role) == Task::open) ? Task::closed : Task::open;
    QVERIFY(this->model->setData(test_index_clone, new_status, active_role));
    QCOMPARE(test_index.data(active_role), new_status);
    TestTaskItemModel::assert_index_equality(test_index, test_index_clone);
}

void TestTaskItemModel::test_remove_rows() {
    const auto index_to_remove = TestHelpers::find_model_index_by_display_role(
        *this->model, "Buy groceries"
    );
    QVERIFY(index_to_remove.isValid());
    const auto index_parent = index_to_remove.parent();
    const int number_of_siblings = this->model->rowCount(index_parent) - 1;
    QVERIFY(this->model->removeRows(index_to_remove.row(), 1, index_parent));
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));
    QCOMPARE(this->model->get_size(), 6);

    QCOMPARE(this->model->rowCount(index_parent), number_of_siblings);
}

void TestTaskItemModel::test_create_task() {
    const auto parent1 = TestHelpers::find_model_index_by_display_role(
        *this->model, "Buy groceries"
    );
    const auto parent2 = TestHelpers::find_model_index_by_display_role(
        *this->model, "Do chores"
    );
    QVERIFY(parent1.isValid());
    QVERIFY(parent2.isValid());

    const QString new_task_title = "My new test task";
    QVERIFY(this->model->create_task(new_task_title, {parent1, parent2}));
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));
    QCOMPARE(this->model->get_size(), 12);

    const auto new_index1 = this->model->index(this->model->rowCount(parent1)-1, 0, parent1);
    const auto new_index2 = this->model->index(this->model->rowCount(parent2)-1, 0, parent2);
    QVERIFY(new_index1.isValid());
    QVERIFY(new_index2.isValid());

    QCOMPARE(new_index1.data(           ),   new_task_title);
    QCOMPARE(new_index1.data(active_role),              Task::open);
    QCOMPARE(new_index1.data(start_role ).toDateTime(), QDateTime());
    QCOMPARE(new_index1.data(due_role   ).toDateTime(), QDateTime());
    TestTaskItemModel::assert_index_equality(new_index1, new_index2);
}

void TestTaskItemModel::test_add_dependency() {
    auto index_buy_groceries = TestHelpers::find_model_index_by_display_role(
        *this->model, "Buy groceries"
    );
    auto index_landlord_mail = TestHelpers::find_model_index_by_display_role(
        *this->model, "Answer landlords mail"
    );
    QVERIFY(index_buy_groceries.isValid());
    QVERIFY(index_landlord_mail.isValid());

    QVERIFY(this->model->add_dependency(index_landlord_mail, index_buy_groceries));
    auto cloned_index = this->model->index(1, 0, index_landlord_mail);
    QCOMPARE(this->model->rowCount(index_landlord_mail), 2);
    TestTaskItemModel::assert_index_equality(cloned_index, index_buy_groceries);
    QCOMPARE(this->model->rowCount(cloned_index), this->model->rowCount(index_buy_groceries));
}

void TestTaskItemModel::test_can_not_create_dependency_cycle() {
    auto parent_index = TestHelpers::find_model_index_by_display_role(
        *this->model, "Buy groceries"
    );
    auto nested_child_index = TestHelpers::find_model_index_by_display_role(
        *this->model, "Fix printer", parent_index
    );
    QVERIFY(parent_index.isValid());
    QVERIFY(nested_child_index.isValid());

    auto child_count_parent = this->model->rowCount(parent_index);
    auto child_count_nested_child = this->model->rowCount(nested_child_index);

    QVERIFY(!this->model->add_dependency(nested_child_index, parent_index));
    QCOMPARE(this->model->rowCount(parent_index), child_count_parent);
    QCOMPARE(this->model->rowCount(nested_child_index), child_count_nested_child);
}

void TestTaskItemModel::assert_initial_dataset_representation_base_model() {
    QCOMPARE(this->model->rowCount(), 3);
    QCOMPARE(this->model->get_size(), 10);
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));

    this->find_task_by_title_and_assert_correctness_of_data(
        "Prepare meal",
        Task::open,
        QDateTime::fromString("2025-12-01 16:00:00", Qt::ISODate),
        QDateTime::fromString("2025-12-01 18:15:00", Qt::ISODate),
        2
    );
    this->find_task_by_title_and_assert_correctness_of_data(
        "Do chores", Task::closed, QDateTime(), QDateTime(), 0
    );
    this->find_task_by_title_and_assert_correctness_of_data(
        "Print shopping list", Task::open, QDateTime(), QDateTime::fromString("2025-12-01 16:00:00", Qt::ISODate), 2
    );
}

void TestTaskItemModel::assert_model_persistence() {
    std::unique_ptr<TaskItemModel> model_reloaded_from_db;

    TestHelpers::setup_item_model(model_reloaded_from_db, this->get_db_connection_name());

    TestHelpers::assert_model_equality(
        *model_reloaded_from_db,
        *this->model,
        {Qt::DisplayRole, uuid_role, active_role, start_role, due_role, details_role},
        TestHelpers::compare_indices_by_uuid
    );
}

void TestTaskItemModel::assert_index_equality(
    const QModelIndex& index1,
    const QModelIndex& index2
) {
    const QSet<int> roles = {
        Qt::DisplayRole, uuid_role, active_role, start_role, due_role, details_role
    };
    for (const int role : roles) {
        QCOMPARE(index1.data(role), index2.data(role));
    }
}

void TestTaskItemModel::find_task_by_title_and_assert_correctness_of_data(
    const QString& title,
    Task::Status status,
    const QDateTime& start_datetime,
    const QDateTime& due_datetime,
    int number_of_children
) {
    auto index = TestHelpers::find_model_index_by_display_role(*this->model, title);
    QVERIFY(index.isValid());
    QCOMPARE(index.data(active_role), status);
    QCOMPARE(index.data(start_role).toDateTime(), start_datetime);
    QCOMPARE(index.data(due_role).toDateTime(), due_datetime);
    QCOMPARE(model->rowCount(index), number_of_children);
}


QTEST_GUILESS_MAIN(TestTaskItemModel)
