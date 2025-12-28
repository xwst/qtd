/**
 * Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
 *
 * This file is part of qtd.
 *
 * qtd is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * qtd is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * qtd. If not, see <https://www.gnu.org/licenses/>.
 */

#include "persistedtreeitemmodelstestbase.h"

#include <QLoggingCategory>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTest>

#include "../testhelpers.h"
#include "utils/initialize.h"

PersistedTreeItemModelsTestBase::PersistedTreeItemModelsTestBase(QObject *parent)
    : QObject{parent}
{}

void PersistedTreeItemModelsTestBase::initTestCase() {

    QLoggingCategory::setFilterRules("qt.modeltest.debug=true");
    initialize_qt_meta_types();

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
    for (const auto& table_name : {"tag_assignments", "tags", "dependencies", "tasks"}) {
        PersistedTreeItemModelsTestBase::clear_table(table_name);
    }
}

void PersistedTreeItemModelsTestBase::clear_table(const QString& table_name) {
    QSqlQuery query;
    query.exec("DELETE FROM " + table_name);
    query.exec("SELECT COUNT(*) FROM " + table_name);
    query.first();
    QCOMPARE(query.value(0), 0);
}

QString PersistedTreeItemModelsTestBase::get_db_connection_name() const {
    return this->db_connection_name;
}
