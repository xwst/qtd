#include "testhelpers.h"

#include <QTest>

#include "../src/app/util.h"

void TestHelpers::setup_database() {
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    Util::create_tables_if_not_exist(db.connectionName());
}

void TestHelpers::assert_table_exists(const QString& table_name) {
    QSqlQuery query;
    bool result = query.exec("SELECT COUNT(*) FROM " + table_name);
    QString error_msg = QString("The '%1'-table was not created!").arg(table_name);
    QVERIFY2(result, qPrintable(error_msg));
}

void TestHelpers::populate_database() {
    QFile f(":/resources/sql/generic/populate_database.sql");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&f);
    QString all_queries_str = in.readAll();

    QSqlQuery query;
    for (QString& query_str : Util::split_queries(all_queries_str))
        QVERIFY2(query.exec(query_str), "Error while populating test database");
}

QSet<QString> TestHelpers::get_display_roles(
    const QAbstractItemModel& model,
    const QModelIndex& parent
) {
    QSet<QString> result;
    if (parent.isValid()) result.insert(parent.data().toString());

    auto column = parent.isValid() ? parent.column() : 0;
    for (int i=0; i<model.rowCount(parent); i++)
        result.unite(get_display_roles(model, model.index(i, column, parent)));
    return result;
}
