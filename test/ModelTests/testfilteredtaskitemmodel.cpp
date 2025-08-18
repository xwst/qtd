#include "testfilteredtaskitemmodel.h"

#include <memory>

#include <QLoggingCategory>
#include <QSqlDatabase>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/model/model_constants.h"

TestFilteredTaskItemModel::TestFilteredTaskItemModel(QObject *parent)
    : QObject{parent}
{}

void TestFilteredTaskItemModel::initTestCase() {

    QLoggingCategory::setFilterRules("qt.modeltest.debug=true");

    QVERIFY(TestHelpers::setup_database());
    TestHelpers::populate_database();

    this->base_model = std::make_unique<TaskItemModel>(
        QSqlDatabase::database().connectionName()
    );
    QCOMPARE(this->base_model->rowCount(), 3);

    TestHelpers::setup_proxy_item_model(this->model, this->base_model.get(), nullptr);
}

void TestFilteredTaskItemModel::test_filter_single_word() {
    this->model->set_search_string("buy");
    QCOMPARE(this->model->rowCount(), 1);
    const auto remaining_index = this->model->index(0, 0);
    QCOMPARE(this->model->data(remaining_index), "Buy groceries");
    QCOMPARE(this->model->rowCount(remaining_index), 0);
}

void TestFilteredTaskItemModel::test_filter_multiple_words() {
    this->model->set_search_string("Answer mail");
    QCOMPARE(this->model->rowCount(), 1);
    const auto remaining_index = this->model->index(0, 0);
    QCOMPARE(this->model->data(remaining_index), "Answer landlords mail");
    QCOMPARE(this->model->rowCount(remaining_index), 0);
}

void TestFilteredTaskItemModel::test_filter_with_quotes() {
    const QString task1 = "Dummy task with short description";
    const QString task2 = "Another task with a dummy description";
    this->base_model->create_task(task1);
    this->base_model->create_task(task2);


    this->model->set_search_string("dummy task");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        QStringList({task1, task2})
    );

    this->model->set_search_string("\"dummy task\"");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        QStringList({task1})
    );
}

void TestFilteredTaskItemModel::test_filter_for_task_details() {
    this->model->set_search_string("TOOTHPASTE");
    QCOMPARE(this->model->rowCount(), 1);

    const auto remaining_index = this->model->index(0, 0);
    QCOMPARE(remaining_index.data(uuid_role), "dc1f5ff8-db45-6630-9340-13a7d860d910");
    QCOMPARE(this->model->rowCount(remaining_index), 0);
}

void TestFilteredTaskItemModel::test_no_matches() {
    this->model->set_search_string("55fe5a86-d010-4a31-8016-d25034921f30");
    QCOMPARE(this->model->rowCount(), 0);
}

void TestFilteredTaskItemModel::test_no_filter() {
    this->model->clear_search_string();
    TestHelpers::assert_model_equality(
        *this->model,
        *this->base_model,
        {Qt::DisplayRole, uuid_role},
        TestHelpers::compare_indices_by_uuid
    );
}

void TestFilteredTaskItemModel::test_filter_independent_of_filter_word_order() {
    this->model->set_search_string("a e p");
    const auto filtered_items = TestHelpers::get_display_roles(*this->model);

    this->model->set_search_string("e a p");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        filtered_items
    );
}

void TestFilteredTaskItemModel::test_repeat_words_has_no_effect() {
    this->model->set_search_string("a e p");
    const auto filtered_items = TestHelpers::get_display_roles(*this->model);

    this->model->set_search_string("a a e p a p");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        filtered_items
    );
}

void TestFilteredTaskItemModel::test_modifying_base_model_propagates_to_proxy() {
    this->model->set_search_string("print");
    const auto base_index = TestHelpers::find_model_index_by_display_role(*this->base_model, "Fix printer");
    const auto proxy_index = this->model->mapFromSource(base_index);
    QCOMPARE(proxy_index.data(), base_index.data());

    const QString new_description = "Fix printer issues";
    this->base_model->setData(base_index, new_description, Qt::DisplayRole);
    QCOMPARE(proxy_index.data(), new_description);

    this->base_model->removeRow(base_index.row(), base_index.parent());
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        QStringList({"Print recipe", "Print shopping list"})
    );

    const QString new_index_description = "index-not-yet-created";
    this->model->set_search_string(new_index_description);
    QCOMPARE(this->model->rowCount(), 0);
    this->base_model->create_task(
        new_index_description,
        {this->base_model->index(2, 0)}
    );
    QCOMPARE(this->model->rowCount(), 1);
    const auto new_proxy_index = this->model->index(0, 0);
    QCOMPARE(this->model->rowCount(new_proxy_index), 0);
    QCOMPARE(new_proxy_index.data(), new_index_description);
}

void TestFilteredTaskItemModel::test_parents_become_childless_if_no_child_matches() {
    this->model->set_search_string("meal");
    QCOMPARE(
        this->model->rowCount(this->model->index(0, 0)),
        0
    );
}

void TestFilteredTaskItemModel::test_matching_children_are_kept_if_parents_are_filtered_out() {
    const QString index_title = "Check food supplies";
    this->model->set_search_string(index_title);
    const auto index = TestHelpers::find_model_index_by_display_role(*this->model, index_title);
    QCOMPARE(index.data(), index_title);
    QCOMPARE(index.parent(), QModelIndex());
}

QTEST_GUILESS_MAIN(TestFilteredTaskItemModel)
