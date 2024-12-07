#include "testtagmodels.h"

#include <QSqlDataBase>
#include <QSqlQuery>

#include "../testhelpers.h"

TestTagModels::TestTagModels(QObject *parent)
    : QObject{parent}
{}

void TestTagModels::initTestCase() {
    TestHelpers::setup_database();
    TestHelpers::assert_table_exists("tags");
}

void TestTagModels::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestTagModels::init() {
    TestHelpers::populate_database();
    QString connection_name = QSqlDatabase::database().connectionName();
    this->model = std::make_unique<TagItemModel>(connection_name);
}

void TestTagModels::cleanup() {
    QSqlQuery("TRUNCATE TABLE tags");
    this->model.release();
}


void TestTagModels::initial_dataset_represented_correctly() {
}

QTEST_MAIN(TestTagModels)
