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

#include "testfilteredtagitemmodel.h"

#include <memory>

#include <QSet>
#include <QSqlDatabase>
#include <QTest>
#include <QUuid>

#include "../../src/app/model/filteredtagitemmodel.h"
#include "../../src/app/model/tagitemmodel.h"
#include "../testhelpers.h"

TestFilteredTagItemModel::TestFilteredTagItemModel(QObject *parent)
    : QObject{parent}
{}

void TestFilteredTagItemModel::initTestCase() {
    QVERIFY(TestHelpers::setup_database());
    TestHelpers::populate_database();

    this->base_model = std::make_unique<TagItemModel>(
        QSqlDatabase::database().connectionName()
    );
    this->model = std::make_unique<FilteredTagItemModel>();
    this->model->setSourceModel(this->base_model.get());
}

void TestFilteredTagItemModel::test_empty_whitelist_yields_empty_model() const {
    this->model->set_tag_whitelist(QSet<QUuid>());
    QCOMPARE(this->model->rowCount(), 0);
}

void TestFilteredTagItemModel::test_whitelisting_parent() const {
    this->whitelist_single_parent_tag_and_validate();
}

void TestFilteredTagItemModel::test_whitelisting_children() const {
    this->model->set_tag_whitelist( {
        QUuid::fromString("b7f5d20c-3ea7-4d20-86e2-3c682fc05756"),
        QUuid::fromString("3fd213cf-1c50-47a0-9237-c2da78bf4fcc")
    });
    QCOMPARE(this->model->rowCount(), 2);

    auto index = this->model->index(0, 0);

    QCOMPARE(index.data().toString(), "Private");
    QCOMPARE(this->model->rowCount(index), 1);

    index = this->model->index(0, 0, index);
    QCOMPARE(index.data().toString(), "Fun");
    QCOMPARE(this->model->rowCount(index), 1);

    index = this->model->index(0, 0, index);
    QCOMPARE(index.data().toString(), "Vacation");
    QCOMPARE(this->model->rowCount(index), 0);

    index = this->model->index(1, 0);
    QCOMPARE(index.data().toString(), "Work");
    QCOMPARE(this->model->rowCount(index), 1);

    index = this->model->index(0, 0, index);
    QCOMPARE(index.data().toString(), "Mails");
    QCOMPARE(this->model->rowCount(index), 0);
}

void TestFilteredTagItemModel::test_removing_tags_from_whitelist() const {
    this->model->set_tag_whitelist( {
        QUuid::fromString("b7f5d20c-3ea7-4d20-86e2-3c682fc05756"),
        QUuid::fromString("3fd213cf-1c50-47a0-9237-c2da78bf4fcc")
    });

    QCOMPARE(this->model->rowCount(), 2);
    this->whitelist_single_parent_tag_and_validate();
}

void TestFilteredTagItemModel::whitelist_single_parent_tag_and_validate() const {
    this->model->set_tag_whitelist(
        { QUuid::fromString("c99586cb-3910-4fab-b5a4-d936c9e58471") }
        );
    QCOMPARE(this->model->rowCount(), 1);

    auto index = this->model->index(0, 0);
    QCOMPARE(index.data().toString(), "Work");
    QCOMPARE(this->model->rowCount(index), 0);
}

QTEST_GUILESS_MAIN(TestFilteredTagItemModel)
