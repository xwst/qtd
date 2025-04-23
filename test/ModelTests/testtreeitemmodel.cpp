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

#include "testtreeitemmodel.h"

#include <memory>
#include <utility>

#include <QAbstractItemModelTester>
#include <QObject>
#include <QSet>
#include <QSignalSpy>
#include <QTest>
#include <QVariant>

#include "../../src/app/model/model_constants.h"
#include "../../src/app/util.h"
#include "../testhelpers.h"
#include "testmodelwrappers.h"

TestTreeItemModel::TestTreeItemModel(QObject *parent)
    : QObject{parent}
{}

void TestTreeItemModel::init() {
    this->setup_initial_model();
}

void TestTreeItemModel::cleanup() {
    this->model.reset();
}

void TestTreeItemModel::setup_initial_model() {
    this->model = std::make_unique<TreeItemModelTestWrapper>();
    new QAbstractItemModelTester(this->model.get(), this->model.get());

    QVERIFY(this->model->create_tree_node(std::make_unique<TestHelpers::TestTag>("A")));
    QVERIFY(this->model->create_tree_node(std::make_unique<TestHelpers::TestTag>("B")));
    const auto B_index = this->model->index(1, 0);
    QVERIFY(this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("B1"),
        B_index.data(uuid_role).toUuid()
    ));
    QCOMPARE(this->model->get_size(), 3);
}

void TestTreeItemModel::test_initial_setup() {
    const auto A_index = this->model->index(0, 0);
    const auto B_index = this->model->index(1, 0);
    const auto B1_index = this->model->index(0, 0, B_index);

    this->verify_item(A_index,  "A",  0, QModelIndex());
    this->verify_item(B_index,  "B",  1, QModelIndex());
    this->verify_item(B1_index, "B1", 0, B_index);
}

void TestTreeItemModel::test_set_data() {
    QVERIFY(!this->model->setData(QModelIndex(), ""));

    const auto A_index = this->model->index(0, 0);
    QVERIFY(!this->model->setData(A_index, "new name", Qt::ToolTipRole)); // Unused role
    QCOMPARE(A_index.data(), "A");

    QVERIFY(this->model->setData(A_index, "new name", Qt::DisplayRole));
    QCOMPARE(A_index.data(), "new name");

    const auto A_uuid = A_index.data(uuid_role);
    QVERIFY(!this->model->setData(A_index, QUuid::createUuid(), uuid_role));
    QCOMPARE(A_index.data(uuid_role), A_uuid);
}

void TestTreeItemModel::test_remove_single_row() {
    const auto initial_size = this->model->get_size();
    auto data = std::make_unique<TestHelpers::TestTag>("about to be deleted");
    const auto *data_ptr = data.get();
    QSignalSpy spy(data_ptr, SIGNAL(destroyed(QObject*)));
    this->model->create_tree_node(std::move(data));
    this->model->create_tree_node(std::make_unique<TestHelpers::TestTag>("C"));

    QVERIFY(this->model->removeRows(2, 1));
    QCOMPARE(this->model->get_size(), initial_size + 1);
    QCOMPARE(this->model->rowCount(), initial_size);
    this->test_initial_setup();

    auto C_index = this->model->index(2, 0);
    this->verify_item(C_index,  "C",  0, QModelIndex());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).value<QObject*>(), data_ptr);

    auto B_index = this->model->index(1, 0);
    QVERIFY(this->model->removeRows(0, 1, B_index));
    QCOMPARE(this->model->get_size(), initial_size);
    QCOMPARE(this->model->rowCount(), initial_size);
    QCOMPARE(this->model->rowCount(B_index), 0);
}

void TestTreeItemModel::test_remove_multiple_rows() {
    const auto initial_size = this->model->get_size();
    auto first_new_item  = std::make_unique<TestHelpers::TestTag>("about to be deleted");
    auto second_new_item = std::make_unique<TestHelpers::TestTag>("about to be deleted, too");

    const auto *first_data_pointer = first_new_item.get();
    const auto *second_data_pointer = second_new_item.get();

    auto first_spy = QSignalSpy(first_data_pointer, SIGNAL(destroyed(QObject*)));
    auto second_spy = QSignalSpy(second_data_pointer, SIGNAL(destroyed(QObject*)));

    this->model->create_tree_node(std::move(first_new_item));
    this->model->create_tree_node(std::move(second_new_item));

    QVERIFY(this->model->removeRows(2, 2));
    QCOMPARE(this->model->get_size(), initial_size);
    QCOMPARE(this->model->rowCount(), initial_size-1);
    this->test_initial_setup();

    QCOMPARE(first_spy.count(), 1);
    QCOMPARE(first_spy.takeFirst().at(0).value<QObject*>(), first_data_pointer);
    QCOMPARE(second_spy.count(), 1);
    QCOMPARE(second_spy.takeFirst().at(0).value<QObject*>(), second_data_pointer);
}

void TestTreeItemModel::test_remove_rows_with_children() {
    this->model->create_tree_node(std::make_unique<TestHelpers::TestTag>("C"));

    QVERIFY(this->model->removeRows(0, 2));
    QCOMPARE(this->model->get_size(), 1);
    QCOMPARE(this->model->rowCount(), 1);
    QCOMPARE(this->model->index(0, 0).data().toString(), "C");
}

void TestTreeItemModel::test_clone_tree_node_clones_children_recursiveley() {
    auto A_index = this->model->index(0, 0);
    auto B_index = this->model->index(1, 0);
    auto B1_index = this->model->index(0, 0, B_index);
    auto B1_uuid = this->model->index(0, 0, B_index).data(uuid_role).toUuid();

    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 1"), B1_uuid
    );
    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 2"), B1_uuid
    );
    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 1a"),
        this->model->index(0, 0, B1_index).data(uuid_role).toUuid()
    );

    QVERIFY(
        this->model->clone_tree_node(
            B_index.data(uuid_role).toUuid(),
            A_index.data(uuid_role).toUuid()
        )
    );
    auto clone_index = this->model->index(0, 0, A_index);

    auto child1a_index = this->model->index(0, 0, this->model->index(0, 0, B1_index));
    this->model->setData(child1a_index, "modified name for child 1a", Qt::DisplayRole);

    TestHelpers::assert_model_equality(
        *this->model,
        *this->model,
        {Qt::DisplayRole, Qt::DecorationRole, uuid_role},
        TestHelpers::compare_indices_by_uuid,
        B_index,
        clone_index
    );
    QCOMPARE(this->model->get_size(), Util::count_model_rows(this->model.get()));
}

void TestTreeItemModel::test_adding_children_to_clones() {
    auto B_index = this->model->index(1, 0);
    auto B1_index = this->model->index(0, 0, B_index);
    auto B1_uuid = this->model->index(0, 0, B_index).data(uuid_role).toUuid();

    QVERIFY(this->model->clone_tree_node(B1_uuid));
    auto clone_index = this->model->index(2, 0);
    QCOMPARE(clone_index.parent(), QModelIndex());
    TestHelpers::assert_index_equality(
        B1_index,
        clone_index,
        {Qt::DisplayRole, static_cast<Qt::ItemDataRole>(uuid_role)}
    );
    QCOMPARE(this->model->rowCount(B1_index), 0);

    QSignalSpy spy(this->model.get(), SIGNAL(rowsInserted(const QModelIndex&, int, int)));
    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 1"), B1_uuid
    );
    TestHelpers::assert_index_equality(
        B1_index,
        clone_index,
        {Qt::DisplayRole, static_cast<Qt::ItemDataRole>(uuid_role)}
    );
    QCOMPARE(this->model->rowCount(B1_index), 1);
    QCOMPARE(spy.count(), 2);

    const QSet<QModelIndex> expected_signalling_indices = {B1_index, clone_index};
    QCOMPARE(this->model_indices_of_row_change_signals(spy), expected_signalling_indices);
    spy.clear();

    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 2"),
        clone_index.data(uuid_role).toUuid()
    );
    TestHelpers::assert_index_equality(
        B1_index,
        clone_index,
        {Qt::DisplayRole, static_cast<Qt::ItemDataRole>(uuid_role)}
    );
    QCOMPARE(this->model->rowCount(B1_index), 2);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(this->model_indices_of_row_change_signals(spy), expected_signalling_indices);
}

void TestTreeItemModel::test_remove_clone() {
    const auto B_index = this->model->index(1, 0);
    const auto B1_index = this->model->index(0, 0, B_index);
    const auto B1_uuid = this->model->index(0, 0, B_index).data(uuid_role).toUuid();

    QVERIFY(this->model->clone_tree_node(B1_uuid));
    const auto clone_index = this->model->index(2, 0);
    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 1"),
        B1_index.data(uuid_role).toUuid()
    );

    this->model->removeRows(B1_index.row(), 1, B_index);
    QCOMPARE(this->model->rowCount(), 3);
    QCOMPARE(this->model->get_size(), 4);
    QCOMPARE(this->model->rowCount(clone_index), 1);
    QCOMPARE(this->model->index(0, 0, clone_index).data(), "child 1");
}

void TestTreeItemModel::test_remove_child_of_clone() {
    const auto B_index = this->model->index(1, 0);
    const auto B1_index = this->model->index(0, 0, B_index);
    const auto B1_uuid = this->model->index(0, 0, B_index).data(uuid_role).toUuid();

    QVERIFY(this->model->clone_tree_node(B1_uuid));
    const auto clone_index = this->model->index(2, 0);
    this->model->create_tree_node(
        std::make_unique<TestHelpers::TestTag>("child 1"),
        B1_index.data(uuid_role).toUuid()
    );

    const QSignalSpy spy(this->model.get(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)));

    this->model->removeRows(0, 1, clone_index);
    QCOMPARE(this->model->rowCount(clone_index), 0);
    QCOMPARE(this->model->rowCount(B1_index), 0);
    QCOMPARE(spy.count(), 2);
    const QSet<QModelIndex> expected_signalling_indices = {B1_index, clone_index};
    QCOMPARE(this->model_indices_of_row_change_signals(spy), expected_signalling_indices);
}

void TestTreeItemModel::verify_item(
    const QModelIndex& item, const QString& name, int child_count, const QModelIndex& parent
) {
    QCOMPARE(item.data().toString(), name);
    QCOMPARE(this->model->rowCount(item), child_count);
    QCOMPARE(this->model->columnCount(item), 1);
    QCOMPARE(item.parent(), parent);
    if (parent != QModelIndex()) {
        QCOMPARE(this->model->index(item.row(), 0, parent), item);
    }
}

QSet<QModelIndex> TestTreeItemModel::model_indices_of_row_change_signals(const QSignalSpy& spy) {
    QSet<QModelIndex> result;
    for (const auto& call_arguments : spy) {
        result.insert(call_arguments.at(0).toModelIndex());
    }
    return result;
}

QTEST_GUILESS_MAIN(TestTreeItemModel)
