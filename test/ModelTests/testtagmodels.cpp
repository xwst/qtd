#include "testtagmodels.h"

#include <QSqlDataBase>
#include <QSqlQuery>

#include "../../src/app/util.h"

TestTagModels::TestTagModels(QObject *parent)
    : QObject{parent}
{}

void TestTagModels::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    Util::create_tables_if_not_exist(db.connectionName());
}

void TestTagModels::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestTagModels::init() {

}

void TestTagModels::cleanup() {

}


void TestTagModels::tag_table_exists() {
    QSqlQuery query;
    bool result = query.exec("SELECT COUNT(*) FROM tags");
    QVERIFY2(result, "The 'tags'-table was not created!");
}

QTEST_MAIN(TestTagModels)
