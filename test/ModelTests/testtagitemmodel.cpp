#include "testtagitemmodel.h"

#include <QAbstractItemModelTester>
#include <QSqlDataBase>
#include <QSqlQuery>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/util.h"

TestTagItemModel::TestTagItemModel(QObject *parent)
    : QObject{parent}
{}

void TestTagItemModel::initTestCase() {
    TestHelpers::setup_database();
    TestHelpers::assert_table_exists("tags");
}

void TestTagItemModel::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestTagItemModel::init() {
    TestHelpers::populate_database();
    QString connection_name = QSqlDatabase::database().connectionName();
    this->model = std::make_unique<TagItemModel>(connection_name);
    new QAbstractItemModelTester(this->model.get(), this->model.get());
}

void TestTagItemModel::cleanup() {
    QSqlQuery query;
    query.exec("DELETE FROM tags");
    query.exec("SELECT COUNT(*) FROM tags");
    query.first();
    QCOMPARE(0, query.value(0));
    this->model.release();
}

void TestTagItemModel::initial_dataset_represented_correctly() {
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

void TestTagItemModel::remove_rows() {
    int total_rows = Util::count_model_rows(this->model.get());
    QModelIndex index_without_child = QModelIndex();
    while (this->model->hasChildren(index_without_child))
        index_without_child = this->model->index(0, 0, index_without_child);

    this->model->removeRow(
        index_without_child.row(),
        index_without_child.parent()
    ); // Remove first row without children
    QCOMPARE(total_rows - 1, Util::count_model_rows(this->model.get()));

    auto first_toplevel_index = this->model->index(0, 0);
    this->model->removeRows(
        0,
        this->model->rowCount(first_toplevel_index),
        first_toplevel_index
    ); // Remove multiple rows including children
    QCOMPARE(4, Util::count_model_rows(this->model.get()));

    // todo: Check that the correct rows were removed.
}

QTEST_MAIN(TestTagItemModel)
