#include "testtaskitemmodels.h"

#include <QAbstractItemModelTester>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/model/model_constants.h"
#include "../../src/app/util.h"

TestTaskItemModel::TestTaskItemModel(QObject *parent)
    : PersistedTreeItemModelsTestBase{parent}
{}

void TestTaskItemModel::initTestCase() {
    PersistedTreeItemModelsTestBase::initTestCase();
    TestHelpers::assert_table_exists("tasks");
}

void TestTaskItemModel::init() {
    PersistedTreeItemModelsTestBase::init();
    TestHelpers::setup_item_model(this->model, this->db_connection_name);
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
    QVERIFY(this->model->data(this->model->index(0, 0), document_role).isValid());
}

void TestTaskItemModel::test_data_change_of_unique_task() {
    QString title = "Buy groceries";
    auto test_index = TestHelpers::find_model_index_by_display_role(*this->model, title);
    QVERIFY(test_index.isValid());

    auto due_time = test_index.data(due_role).toDateTime();
    auto uuid = test_index.data(uuid_role).toUuid();
    QCOMPARE(test_index.data(active_role).toBool(), true);

    auto new_start_time = QDateTime::currentDateTime();
    this->model->setData(test_index, new_start_time, start_role);
    this->model->setData(test_index, false, active_role);

    QCOMPARE(test_index.data(active_role).toBool(), false);
    QCOMPARE(test_index.data(start_role).toDateTime(), new_start_time);

    QCOMPARE(test_index.data().toString(), title);
    QCOMPARE(test_index.data(due_role).toDateTime(), due_time);
    QCOMPARE(test_index.data(uuid_role).toUuid(), uuid);
}

void TestTaskItemModel::test_data_change_of_cloned_task() {
    auto test_index = TestHelpers::find_model_index_by_display_role(
        *this->model,
        "Fix printer",
        TestHelpers::find_model_index_by_display_role(
            *this->model,
            "Buy groceries"
        )
    );
    auto test_index_clone = TestHelpers::find_model_index_by_display_role(
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
    this->assert_index_equality(test_index, test_index_clone);

    QString new_title = "Fix printer and refill ink";
    this->model->setData(test_index, new_title);
    QCOMPARE(test_index_clone.data().toString(), new_title);
    this->assert_index_equality(test_index, test_index_clone);

    bool status = test_index_clone.data(active_role).toBool();
    this->model->setData(test_index_clone, !status, active_role);
    QCOMPARE(test_index.data(active_role).toBool(), !status);
    this->assert_index_equality(test_index, test_index_clone);
}

void TestTaskItemModel::test_remove_rows() {
    auto index_to_remove = TestHelpers::find_model_index_by_display_role(
        *this->model, "Buy groceries"
    );
    QVERIFY(index_to_remove.isValid());
    auto index_parent = index_to_remove.parent();
    int number_of_siblings = this->model->rowCount(index_parent) - 1;
    this->model->removeRows(index_to_remove.row(), 0, index_parent);
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));
    QCOMPARE(this->model->get_size(), 7);

    QCOMPARE(this->model->rowCount(index_parent), number_of_siblings);
}

void TestTaskItemModel::test_create_task() {
    auto parent1 = TestHelpers::find_model_index_by_display_role(
        *this->model, "Buy groceries"
    );
    auto parent2 = TestHelpers::find_model_index_by_display_role(
        *this->model, "Do chores"
    );
    QString new_task_title = "New task";
    QVERIFY(this->model->create_task(new_task_title, {parent1, parent2}));
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));
    QCOMPARE(this->model->get_size(), 12);

    auto new_index1 = this->model->index(this->model->rowCount(parent1), 0, parent1);
    auto new_index2 = this->model->index(this->model->rowCount(parent2), 0, parent2);

    QCOMPARE(new_index1.data(           ).toString(),   new_task_title);
    QCOMPARE(new_index1.data(active_role).toBool(),     true);
    QCOMPARE(new_index1.data(start_role ).toDateTime(), QDateTime());
    QCOMPARE(new_index1.data(due_role   ).toDateTime(), QDateTime());
    this->assert_index_equality(new_index1, new_index2);
}

void TestTaskItemModel::assert_initial_dataset_representation_base_model() {
    QCOMPARE(this->model->rowCount(), 3);
    QCOMPARE(this->model->get_size(), 10);
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));

    this->find_task_by_title_and_assert_correctness_of_data(
        "Prepare meal",
        true,
        QDateTime::fromString("2025-12-01 16:00:00", Qt::ISODate),
        QDateTime::fromString("2025-12-01 18:15:00", Qt::ISODate),
        2
    );
    this->find_task_by_title_and_assert_correctness_of_data(
        "Do chores", false, QDateTime(), QDateTime(), 0
    );
    this->find_task_by_title_and_assert_correctness_of_data(
        "Print shopping list", true, QDateTime(), QDateTime::fromString("2025-12-01 16:00:00", Qt::ISODate), 2
    );
}

void TestTaskItemModel::assert_model_persistence() {
    std::unique_ptr<TaskItemModel> model_reloaded_from_db;

    TestHelpers::setup_item_model(model_reloaded_from_db, this->db_connection_name);

    TestHelpers::assert_model_equality(
        *model_reloaded_from_db.get(),
        *this->model.get(),
        {Qt::DisplayRole, uuid_role, active_role, start_role, due_role, document_role},
        PersistedTreeItemModelsTestBase::compare_indices
    );
}

void TestTaskItemModel::assert_index_equality(
    const QModelIndex& index1,
    const QModelIndex& index2
) {
    QSet<int> roles = {Qt::DisplayRole, uuid_role, active_role, start_role, due_role, document_role};
    for (int role : roles)
        QCOMPARE(index1.data(role), index2.data(role));
}

void TestTaskItemModel::find_task_by_title_and_assert_correctness_of_data(
    QString title, bool is_active, QDateTime start_datetime, QDateTime due_datetime, int number_of_children
) {
    auto index = TestHelpers::find_model_index_by_display_role(*this->model, title);
    QVERIFY(index.isValid());
    QCOMPARE(index.data(active_role).toBool(),    is_active);
    QCOMPARE(index.data(start_role).toDateTime(), start_datetime);
    QCOMPARE(index.data(due_role).toDateTime(),   due_datetime);
    QCOMPARE(model->rowCount(index), number_of_children);
}


QTEST_GUILESS_MAIN(TestTaskItemModel)
