#include "testtagitemmodel.h"

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
}

void TestTagItemModel::cleanup() {
    QSqlQuery("TRUNCATE TABLE tags");
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

QTEST_MAIN(TestTagItemModel)
