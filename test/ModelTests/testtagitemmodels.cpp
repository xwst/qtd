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
    QCOMPARE(0, query.value(0));
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
    QCOMPARE(initial_row_number - 1, Util::count_model_rows(base_model));
    QCOMPARE(initial_row_number - 1, Util::count_model_rows(this->flat_model.get()));
}

void TestTagItemModels::test_remove_single_row_with_children() {
    auto base_model = this->model.get();
    this->remove_first_top_level_row(*base_model);
    QCOMPARE(4, Util::count_model_rows(base_model));
    QCOMPARE(4, Util::count_model_rows(this->flat_model.get()));

    // todo: Check that the correct rows were removed.
}

void TestTagItemModels::assert_initial_dataset_representation_base_model() {
    QCOMPARE(2, this->model->rowCount());
    for (int row=0; row<this->model->rowCount(); row++) {
        auto row_index = this->model->index(row, 0);
        if (row_index.data() == "Fruits")
            QCOMPARE(4, this->model->rowCount(row_index));
        else if (row_index.data() == "Vegetables")
            QCOMPARE(2, this->model->rowCount(row_index));
        else QFAIL("Unexpected model entry at top level!");
    }
    QCOMPARE(10, Util::count_model_rows(this->model.get()));
}

void TestTagItemModels::assert_initial_dataset_representation_flat_model() {
    int total_source_rows = Util::count_model_rows(this->model.get());
    int total_rows = Util::count_model_rows(this->flat_model.get());
    int top_level_rows = this->flat_model->rowCount();
    QCOMPARE(total_source_rows, total_rows);
    QCOMPARE(total_source_rows, top_level_rows);

    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto row_index = this->flat_model->index(row, 0);
        QCOMPARE(0, this->flat_model->rowCount(row_index));
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

void TestTagItemModels::remove_first_top_level_row(QAbstractItemModel& model) {
    auto first_toplevel_index = model.index(0, 0);
    model.removeRows(
        0,
        model.rowCount(first_toplevel_index),
        first_toplevel_index
        ); // Remove multiple rows including children
}

QTEST_MAIN(TestTagItemModels)
