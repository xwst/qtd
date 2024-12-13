#include "testtagitemmodels.h"

#include <QAbstractItemModelTester>
#include <QSqlDataBase>
#include <QSqlQuery>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/util.h"

TestTagItemModels::TestTagItemModels(QObject *parent)
    : QObject{parent}
{}

void TestTagItemModels::initTestCase() {
    TestHelpers::setup_database();
    TestHelpers::assert_table_exists("tags");
}

void TestTagItemModels::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestTagItemModels::init() {
    TestHelpers::populate_database();
    QString connection_name = QSqlDatabase::database().connectionName();

    TestHelpers::setup_item_model(this->model, connection_name);
    TestHelpers::setup_proxy_item_model(this->flat_model, this->model.get());
}

void TestTagItemModels::cleanup() {
    QSqlQuery query;
    query.exec("DELETE FROM tags");
    query.exec("SELECT COUNT(*) FROM tags");
    query.first();
    QCOMPARE(query.value(0), 0);
    this->model.release();
    this->flat_model.release();

}

void TestTagItemModels::test_initial_dataset_represented_correctly() {
    this->assert_initial_dataset_representation_base_model();
    this->assert_initial_dataset_representation_flat_model();
}

void TestTagItemModels::test_remove_single_row() {
    auto base_model = this->model.get();
    int initial_row_number = Util::count_model_rows(base_model);
    remove_single_row_without_children(*base_model);
    QCOMPARE(Util::count_model_rows(base_model), initial_row_number - 1);
    QCOMPARE(Util::count_model_rows(this->flat_model.get()), initial_row_number - 1);
}

void TestTagItemModels::test_remove_rows_with_children() {
    auto base_model = this->model.get();
    this->remove_children_of_first_top_level_index(*base_model);
    QCOMPARE(Util::count_model_rows(base_model), 4);
    QCOMPARE(Util::count_model_rows(this->flat_model.get()), 4);

    QSet<QString> remaining_expected = {"Fruits", "Vegetables", "Carrots", "Chickpeas"};
    auto remaining = TestHelpers::get_display_roles(*base_model);
    auto remaining_flat = TestHelpers::get_display_roles(*this->flat_model.get());
    QCOMPARE(remaining, remaining_expected);
    QCOMPARE(remaining, remaining_expected);
}

void TestTagItemModels::assert_initial_dataset_representation_base_model() {
    QCOMPARE(this->model->rowCount(), 2);
    for (int row=0; row<this->model->rowCount(); row++) {
        auto row_index = this->model->index(row, 0);
        if (row_index.data() == "Fruits")
            QCOMPARE(this->model->rowCount(row_index), 4);
        else if (row_index.data() == "Vegetables")
            QCOMPARE(this->model->rowCount(row_index), 2);
        else QFAIL("Unexpected model entry at top level!");
    }
    QCOMPARE(Util::count_model_rows(this->model.get()), 10);
}

void TestTagItemModels::assert_initial_dataset_representation_flat_model() {
    int total_source_rows = Util::count_model_rows(this->model.get());
    int total_rows = Util::count_model_rows(this->flat_model.get());
    int top_level_rows = this->flat_model->rowCount();
    QCOMPARE(total_rows, total_source_rows);
    QCOMPARE(top_level_rows, total_source_rows);

    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto row_index = this->flat_model->index(row, 0);
        QCOMPARE(this->flat_model->rowCount(row_index), 0);
    }
}

void TestTagItemModels::remove_single_row_without_children(QAbstractItemModel& model) {
    QModelIndex index_without_child = QModelIndex();
    while (model.hasChildren(index_without_child))
        index_without_child = model.index(0, 0, index_without_child);

    model.removeRow(
        index_without_child.row(),
        index_without_child.parent()
        ); // Remove first row without children
}

void TestTagItemModels::remove_children_of_first_top_level_index(QAbstractItemModel& model) {
    auto first_toplevel_index = model.index(0, 0);
    model.removeRows(
        0,
        model.rowCount(first_toplevel_index),
        first_toplevel_index
        ); // Remove multiple rows including children
}

QTEST_MAIN(TestTagItemModels)
