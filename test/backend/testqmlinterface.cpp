/**
 * Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include "testqmlinterface.h"

#include <initializer_list>
#include <memory>

#include <QAbstractItemModel>
#include <QAbstractItemModelTester>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QTest>

#include "../qmlinterface.h"
#include "../testhelpers.h"
#include "utils/initialize.h"

TestQmlInterface::TestQmlInterface(QObject *parent)
    : QObject{parent}
{}

void TestQmlInterface::set_up_db() {
    this->temp_db_file.open();
    auto database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(this->temp_db_file.fileName());
    this->temp_db_file.close();
    database.open();

    QVERIFY(TestHelpers::setup_database());
    TestHelpers::populate_database();
}

void TestQmlInterface::initTestCase() {
    initialize_qt_meta_types();
    this->set_up_db();
    this->qml_interface = std::make_unique<QmlInterface>();
    this->qml_interface->set_up(this->temp_db_file.fileName());
    set_up_model_testers({
        "open_tasks",
        "actionable_tasks",
        "project_tasks",
        "archived_tasks"
    });
}

void TestQmlInterface::set_up_model_testers(std::initializer_list<const char*> model_names) {
    for (const auto* model_name : model_names) {
        auto* model = this->get_model(model_name);
        new QAbstractItemModelTester(model, model); // NOLINT (cppcoreguidelines-owning-memory)
    }
}

QAbstractItemModel* TestQmlInterface::get_model(const char *model_name) const {
    return this->qml_interface->property(model_name).value<QAbstractItemModel*>();
}

void TestQmlInterface::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestQmlInterface::test_model_size() const {
    QCOMPARE(this->get_model("open_tasks")->rowCount(), 2);
    QCOMPARE(this->get_model("actionable_tasks")->rowCount(), 1);
    QCOMPARE(this->get_model("project_tasks")->rowCount(), 2);
    QCOMPARE(this->get_model("archived_tasks")->rowCount(), 2);
}

void TestQmlInterface::test_tag_filtering() const {
    auto tags_open = TestHelpers::get_display_roles(*this->get_model("tags_open"));
    QCOMPARE(tags_open.size(), 4);
    QVERIFY(tags_open.contains("Hobbies"));
    QVERIFY(tags_open.contains("Shopping"));

    auto tags_actionable = TestHelpers::get_display_roles(*this->get_model("tags_actionable"));
    QCOMPARE(tags_actionable.size(), 0);

    auto tags_project = TestHelpers::get_display_roles(*this->get_model("tags_project"));
    QCOMPARE(tags_project.size(), 4);
    QVERIFY(tags_project.contains("Hobbies"));
    QVERIFY(tags_project.contains("Shopping"));

    auto tags_archived = TestHelpers::get_display_roles(*this->get_model("tags_archived"));
    QCOMPARE(tags_archived.size(), 3);
    QVERIFY(tags_archived.contains("Chores"));
    QVERIFY(tags_archived.contains("Shopping"));
}

QTEST_MAIN(TestQmlInterface)
