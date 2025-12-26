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

#include "testtreenodes.h"

#include <memory>
#include <utility>

#include <QObject>
#include <QSignalSpy>
#include <QString>
#include <QTest>

#include "../testhelpers.h"
#include "dataitems/qtdid.h"
#include "dataitems/qtditemdatarole.h"
#include "dataitems/uniquedataitem.h"

TestTreeNodes::TestTreeNodes(QObject *parent)
    : QObject{parent}
{}

void TestTreeNodes::init() {
    this->setup_dummies();
    this->verify_dummies();
}

void TestTreeNodes::cleanup() {
    this->root.reset();
}

void TestTreeNodes::test_add_child() {
  // Dummy function. Test already carried out in init().
}

void TestTreeNodes::test_remove_single_child() {
    const auto initial_child_count = this->root->get_child_count();
    this->root->remove_children(0, 0);
    QCOMPARE(this->root->get_child_count(), initial_child_count);

    auto new_item_data = std::make_unique<TestHelpers::TestTag>("about to be deleted");
    QObject* data_ptr = new_item_data.get();
    this->root->add_child(std::move(new_item_data));
    QCOMPARE(this->root->get_child_count(), initial_child_count+1);

    QSignalSpy spy(data_ptr, SIGNAL(destroyed(QObject*)));

    this->root->remove_children(initial_child_count, 1);
    QCOMPARE(this->root->get_child_count(), initial_child_count);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).value<QObject*>(), data_ptr);
}

void TestTreeNodes::test_remove_multiple_children() {
    auto* node_B = this->root->get_child(1);
    const QString name_pattern = "B%1";
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    for (int i=2; i<10; i++) {
        node_B->add_child(std::make_unique<TestHelpers::TestTag>(name_pattern.arg(i)));
    }
    QCOMPARE(node_B->get_child_count(), 9);
    node_B->remove_children(3, 5);
    QCOMPARE(node_B->get_child_count(), 4);
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    TestTreeNodes::verify_item(node_B->get_child(2), "B3", 0, node_B);
    TestTreeNodes::verify_item(node_B->get_child(3), "B9", 0, node_B);
}

void TestTreeNodes::test_remove_child_hierarchy() {
    const auto initial_child_count = this->root->get_child_count();
    auto* node_A = this->root->get_child(0);
    auto new_item_data = std::make_unique<TestHelpers::TestTag>("A1");

    const auto* data_ptr = new_item_data.get();
    const QSignalSpy spy(data_ptr, SIGNAL(destroyed(QObject*)));
    node_A->add_child(std::move(new_item_data));

    QCOMPARE(node_A->get_child_count(), 1);

    this->root->remove_children(0, 1);
    QCOMPARE(this->root->get_child_count(), initial_child_count-1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().at(0).value<QObject*>(), data_ptr);
}

void TestTreeNodes::test_cloning() {
    const auto root_child_count = this->root->get_child_count();
    this->root->add_child(std::make_unique<TestHelpers::TestTag>("C"));
    auto* node_B = this->root->get_child(1);
    const auto B_child_count = node_B->get_child_count();
    const auto* node_C = this->root->get_child(2);

    node_B->add_child(TreeNode::clone(node_C));
    const auto* node_C_clone = node_B->get_child(B_child_count);

    QCOMPARE(this->root->get_child_count(), root_child_count+1);
    QCOMPARE(node_B->get_child_count(), B_child_count+1);
    QCOMPARE(node_C_clone->get_child_count(), node_C->get_child_count());
    QCOMPARE(node_C_clone->get_child_count(), 0);
    QCOMPARE(node_C->get_parent(), this->root.get());
    QCOMPARE(node_C_clone->get_parent(), node_B);
    QCOMPARE(node_C_clone->get_data(Qt::DisplayRole), "C");
}

void TestTreeNodes::test_set_data() {
    auto *node_A = this->root->get_child(0);
    const auto initial_name = node_A->get_data(Qt::DisplayRole);
    const auto uuid = node_A->get_data(uuid_role);

    node_A->set_data("new name", Qt::ToolTipRole); // Unused role
    QCOMPARE(node_A->get_data(Qt::DisplayRole), initial_name);
    QCOMPARE(node_A->get_data(uuid_role), uuid);

    node_A->set_data("new name", Qt::DisplayRole);
    QCOMPARE(node_A->get_data(Qt::DisplayRole), "new name");
    QCOMPARE(node_A->get_data(uuid_role), uuid);

    node_A->set_data(QtdId::create(), uuid_role);
    QCOMPARE(node_A->get_data(Qt::DisplayRole), "new name");
    QCOMPARE(node_A->get_data(uuid_role), uuid);
}

void TestTreeNodes::setup_dummies() {
    this->root = TreeNode::create(std::make_unique<UniqueDataItem>());
    this->root->add_child(std::make_unique<TestHelpers::TestTag>("A"));
    this->root->add_child(TreeNode::create(std::make_unique<TestHelpers::TestTag>("B")));
    this->root->get_child(1)->add_child(std::make_unique<TestHelpers::TestTag>("B1"));
}

void TestTreeNodes::verify_item(TreeNode* item, const QString& name, int child_count, TreeNode* parent) {
    QCOMPARE(item->get_data(Qt::DisplayRole).toString(), name);
    QCOMPARE(item->get_child_count(), child_count);
    QCOMPARE(item->get_parent(), parent);
    if (parent != nullptr) {
        QCOMPARE(parent->get_child(item->get_row_in_parent()), item);
    }
}

void TestTreeNodes::verify_dummies() {
    TestTreeNodes::verify_item(this->root.get(), "", 2, nullptr);
    TestTreeNodes::verify_item(this->root->get_child(0), "A", 0, this->root.get());

    auto *node_B = this->root->get_child(1);
    TestTreeNodes::verify_item(node_B, "B", 1, this->root.get());
    TestTreeNodes::verify_item(node_B->get_child(0), "B1", 0, node_B);
}

QTEST_GUILESS_MAIN(TestTreeNodes)
