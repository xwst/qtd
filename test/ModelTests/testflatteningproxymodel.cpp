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
    QSqlQuery("TRUNCATE TABLE tags");
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

QTEST_MAIN(TestFlatteningProxyModel)
