#include "testflatteningproxymodel.h"

#include <QSqlDataBase>
#include <QSqlQuery>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/util.h"

TestFlatteningProxyModel::TestFlatteningProxyModel(QObject *parent)
    : QObject{parent}
{}

void TestFlatteningProxyModel::initTestCase() {
    TestHelpers::setup_database();
    TestHelpers::assert_table_exists("tags");
}

void TestFlatteningProxyModel::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestFlatteningProxyModel::init() {
    TestHelpers::populate_database();
    QString connection_name = QSqlDatabase::database().connectionName();
    this->tag_model = std::make_unique<TagItemModel>(connection_name);
    this->model = std::make_unique<FlatteningProxyModel>();
    this->model->setSourceModel(this->tag_model.get());
}

void TestFlatteningProxyModel::cleanup() {
    QSqlQuery query;
    query.exec("DELETE FROM tags");
    query.exec("SELECT COUNT(*) FROM tags");
    query.first();
    QCOMPARE(0, query.value(0));
    this->model.release();
    this->tag_model.release();
}

void TestFlatteningProxyModel::initial_dataset_represented_correctly() {
    int total_source_rows = Util::count_model_rows(this->tag_model.get());
    int total_rows = Util::count_model_rows(this->model.get());
    int top_level_rows = this->model->rowCount();
    QCOMPARE(total_source_rows, total_rows);
    QCOMPARE(total_source_rows, top_level_rows);

    for (int row=0; row<this->model->rowCount(); row++) {
        auto row_index = this->model->index(row, 0);
        QCOMPARE(0, this->model->rowCount(row_index));
    }
}

void TestFlatteningProxyModel::remove_rows() {
    int total_rows = Util::count_model_rows(this->model.get());
    QModelIndex index_without_child = QModelIndex();
    while (this->tag_model->hasChildren(index_without_child))
        index_without_child = this->tag_model->index(0, 0, index_without_child);

    this->tag_model->removeRow(
        index_without_child.row(),
        index_without_child.parent()
        ); // Remove first row without children
    QCOMPARE(total_rows - 1, Util::count_model_rows(this->model.get()));

    auto first_toplevel_index = this->tag_model->index(0, 0);
    this->tag_model->removeRows(
        0,
        this->tag_model->rowCount(first_toplevel_index),
        first_toplevel_index
        ); // Remove multiple rows including children
    QCOMPARE(4, Util::count_model_rows(this->model.get()));

    // todo: Check that the correct rows were removed.
}

QTEST_MAIN(TestFlatteningProxyModel)
