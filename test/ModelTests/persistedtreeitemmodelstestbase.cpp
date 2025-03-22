#include "persistedtreeitemmodelstestbase.h"

#include <QLoggingCategory>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/model/model_constants.h"

PersistedTreeItemModelsTestBase::PersistedTreeItemModelsTestBase(QObject *parent)
    : QObject{parent}
{}

void PersistedTreeItemModelsTestBase::initTestCase() {

    QLoggingCategory::setFilterRules("qt.modeltest.debug=true");

    QVERIFY(TestHelpers::setup_database());
    this->db_connection_name = QSqlDatabase::database().connectionName();
}

void PersistedTreeItemModelsTestBase::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void PersistedTreeItemModelsTestBase::init() {
    TestHelpers::populate_database();
}

void PersistedTreeItemModelsTestBase::cleanup() {
    for (auto& table_name : {"tag_assignments", "tags", "dependencies", "tasks"})
        this->clear_table(table_name);
}

void PersistedTreeItemModelsTestBase::clear_table(QString table_name) {
    QSqlQuery query;
    query.exec("DELETE FROM " + table_name);
    query.exec("SELECT COUNT(*) FROM " + table_name);
    query.first();
    QCOMPARE(query.value(0), 0);
}
