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

#include "testfilteredtaskitemmodel.h"

#include <memory>

#include <QLoggingCategory>
#include <QSet>
#include <QSignalSpy>
#include <QSqlDatabase>
#include <QTest>
#include <QUuid>

#include "../../src/app/model/model_constants.h"
#include "../../src/app/model/taskitemmodel.h"
#include "../testhelpers.h"

namespace {

void check_parents(const QAbstractItemModel& model) {
    QList<QPair<QModelIndex, QModelIndex>> expected_parent_child_pairs;
    expected_parent_child_pairs.append(QPair<QModelIndex, QModelIndex>());

    while (!expected_parent_child_pairs.isEmpty()) {
        auto& [parent, child] = expected_parent_child_pairs.last();
        expected_parent_child_pairs.removeLast();
        QCOMPARE(model.parent(child), parent);

        for (int row=0; row<model.rowCount(child); row++) {
            expected_parent_child_pairs.append(
                QPair<QModelIndex, QModelIndex>(child, model.index(row, 0, child))
            );
        }
    }
}

} // namespace


TestFilteredTaskItemModel::TestFilteredTaskItemModel(QObject *parent)
    : QObject{parent}
{}

void TestFilteredTaskItemModel::initTestCase() {
    QLoggingCategory::setFilterRules("qt.modeltest.debug=true");
}

void TestFilteredTaskItemModel::init() {
    QVERIFY(TestHelpers::setup_database());
    TestHelpers::populate_database();

    this->base_model = std::make_unique<TaskItemModel>(
        QSqlDatabase::database().connectionName()
        );
    QCOMPARE(this->base_model->rowCount(), 3);

    TestHelpers::setup_proxy_item_model(this->model, this->base_model.get(), nullptr);
    this->model->clear_search_string();
    this->spy = std::make_unique<QSignalSpy>(
        this->model.get(),
        &FilteredTaskItemModel::filtered_tags_changed
    );
}

void TestFilteredTaskItemModel::test_filter_single_word() {
    this->model->set_search_string("buy");
    QCOMPARE(this->model->rowCount(), 1);
    const auto remaining_index = this->model->index(0, 0);
    QCOMPARE(this->model->data(remaining_index), "Buy groceries");
    QCOMPARE(this->model->rowCount(remaining_index), 0);

    QCOMPARE(this->spy->count(), 1);
    QCOMPARE(
        this->spy->takeFirst().at(0).value<QSet<QUuid>>(),
        { QUuid::fromString("54c1f21d-bb9a-41df-9658-5111e153f745") }
    );
}

void TestFilteredTaskItemModel::test_filter_multiple_words() const {
    this->model->set_search_string("Answer mail");
    QCOMPARE(this->model->rowCount(), 1);
    const auto remaining_index = this->model->index(0, 0);
    QCOMPARE(this->model->data(remaining_index), "Answer landlords mail");
    QCOMPARE(this->model->rowCount(remaining_index), 0);
}

void TestFilteredTaskItemModel::test_filter_with_quotes() {
    const QString task1 = "Dummy task with short description";
    const QString task2 = "Another task with a dummy description";
    this->base_model->create_task(task1);
    this->base_model->create_task(task2);


    this->model->set_search_string("dummy task");
    QCOMPARE(
        TestHelpers::sort(TestHelpers::get_display_roles(*this->model)),
        TestHelpers::sort(QStringList({task1, task2}))
    );

    this->model->set_search_string("\"dummy task\"");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        QStringList({task1})
    );
}

void TestFilteredTaskItemModel::test_filter_for_task_details() {
    this->model->set_search_string("TOOTHPASTE");
    QCOMPARE(this->model->rowCount(), 1);

    const auto remaining_index = this->model->index(0, 0);
    QCOMPARE(remaining_index.data(uuid_role), "dc1f5ff8-db45-6630-9340-13a7d860d910");
    QCOMPARE(this->model->rowCount(remaining_index), 0);
}

void TestFilteredTaskItemModel::test_no_search_string_matches() {
    this->model->set_search_string("55fe5a86-d010-4a31-8016-d25034921f30");
    QCOMPARE(this->model->rowCount(), 0);
}

void TestFilteredTaskItemModel::test_no_filter() {
    this->model->clear_search_string();
    TestHelpers::assert_model_equality(
        *this->model,
        *this->base_model,
        {Qt::DisplayRole, uuid_role},
        TestHelpers::compare_indices_by_uuid
    );
}

void TestFilteredTaskItemModel::test_filter_independent_of_filter_word_order() {
    this->model->set_search_string("a e p");
    const auto filtered_items = TestHelpers::get_display_roles(*this->model);

    this->model->set_search_string("e a p");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        filtered_items
    );
}

void TestFilteredTaskItemModel::test_repeating_words_has_no_effect() {
    this->model->set_search_string("a e p");
    const auto filtered_items = TestHelpers::get_display_roles(*this->model);

    this->model->set_search_string("a a e p a p");
    QCOMPARE(
        TestHelpers::get_display_roles(*this->model),
        filtered_items
    );
}

void TestFilteredTaskItemModel::test_modifying_base_model_propagates_to_proxy() {
    this->model->clear_search_string();
    this->model->set_search_string("print");

    auto base_index = TestHelpers::find_model_index_by_display_role(*this->base_model, "Fix printer");
    const auto proxy_index = this->model->mapFromSource(base_index);
    QCOMPARE(proxy_index.data(), base_index.data());

    const QString new_description = "Fix printer issues";
    this->base_model->setData(base_index, new_description, Qt::DisplayRole);
    QCOMPARE(proxy_index.data(), new_description);

    while (base_index.isValid()) {
        this->base_model->removeRow(base_index.row(), base_index.parent());
        base_index = TestHelpers::find_model_index_by_display_role(*this->base_model, "Fix printer issues");
    }
    QCOMPARE(
        TestHelpers::sort(TestHelpers::get_display_roles(*this->model)),
        TestHelpers::sort(QStringList({"Print recipe", "Print shopping list"}))
    );

    const QString new_index_description = "index-not-yet-created";
    this->model->set_search_string(new_index_description);
    QCOMPARE(this->model->rowCount(), 0);
    this->base_model->create_task(
        new_index_description,
        {this->base_model->index(2, 0)}
    );
    QCOMPARE(this->model->rowCount(), 1);
    const auto new_proxy_index = this->model->index(0, 0);
    QCOMPARE(this->model->rowCount(new_proxy_index), 0);
    QCOMPARE(new_proxy_index.data(), new_index_description);
}

void TestFilteredTaskItemModel::test_adding_children_to_cloned_items_in_base_model() {
    check_parents(*this->model);
    auto cloned_index = TestHelpers::find_model_index_by_display_role(*this->model, "Fix printer");
    QVERIFY(cloned_index.isValid());

    QVERIFY(this->base_model->create_task("child of cloned 1", {cloned_index}));
    QCOMPARE(this->model->rowCount(cloned_index), 1);
    check_parents(*this->model);

    QVERIFY(this->base_model->create_task("child of cloned 2", {this->model->index(1, 0)}));
    check_parents(*this->model);
};

void TestFilteredTaskItemModel::test_parents_become_childless_if_no_child_matches() {
    this->model->set_search_string("meal");
    QCOMPARE(
        this->model->rowCount(this->model->index(0, 0)),
        0
    );
}

void TestFilteredTaskItemModel::test_matching_children_are_kept_if_parents_are_filtered_out() {
    const QString index_title = "Check food supplies";
    this->model->set_search_string(index_title);
    const auto index = TestHelpers::find_model_index_by_display_role(*this->model, index_title);
    QCOMPARE(index.data(), index_title);
    QCOMPARE(index.parent(), QModelIndex());
}

QTEST_GUILESS_MAIN(TestFilteredTaskItemModel)
