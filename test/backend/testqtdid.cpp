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

#include "testqtdid.h"

#include <functional>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTest>
#include <QUuid>

#include "dataitems/qtdid.h"
#include "utils/initialize.h"

TestQtdId::TestQtdId(QObject *parent)
    : QObject{parent}
{}

void TestQtdId::initTestCase() {
    initialize_qt_meta_types();
}

void TestQtdId::test_creation_from_string() {
    const QString test_id_str = "7e4f1320-4682-425d-808a-7ac534fa23cb";
    const auto test_id = QtdId(test_id_str);
    QCOMPARE(test_id.toString(), test_id_str);
    QVERIFY(test_id.is_valid());

    const auto invalid_id = QtdId("non-sense");
    QCOMPARE(invalid_id.toString(), QUuid().toString(QUuid::WithoutBraces));
    QVERIFY(!invalid_id.is_valid());
}

void TestQtdId::helper_test_conversion(
    const std::function<QVariant(QtdId)>& convert_id
) {
    const QString test_id_str = "c7ad2469-c662-43b5-9f3e-3e05ee26efaf";
    const auto test_id = QtdId(test_id_str);
    auto test_variant = convert_id(test_id);

    QVERIFY(test_variant.canConvert<QtdId>());
    QCOMPARE(test_variant.toString(), test_id_str);
    QCOMPARE(test_variant.value<QtdId>(), test_id);
    QCOMPARE(QVariant::fromValue(test_id), test_variant);
    QCOMPARE(QVariant(test_id), test_variant);
}

void TestQtdId::test_implicit_qvariant_conversion() {
    TestQtdId::helper_test_conversion(
        [](const QtdId& test_id) { return QVariant(test_id); }
    );
}

void TestQtdId::test_qvariant_conversion() {
    TestQtdId::helper_test_conversion(
        [](const QtdId& test_id) { return QVariant::fromValue(test_id); }
    );
}

void TestQtdId::test_database_serialization() {
    const auto test_id = QtdId::create();
    auto database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(":memory:");
    database.open();
    // NOLINTNEXTLINE (bugprone-unused-raii)
    QSqlQuery("CREATE TABLE ids (id VARCHAR(36));", database);

    auto store_query = QSqlQuery(database);
    store_query.prepare("INSERT INTO ids VALUES (?);");
    store_query.bindValue(0, test_id);
    store_query.exec();

    auto fetch_query = QSqlQuery("SELECT id FROM ids;", database);
    fetch_query.next();
    QCOMPARE(fetch_query.value(0).value<QtdId>(), test_id);
}

QTEST_GUILESS_MAIN(TestQtdId)
