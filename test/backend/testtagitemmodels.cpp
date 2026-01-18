/**
 * Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include "testtagitemmodels.h"

#include <functional>
#include <memory>

#include <QAbstractItemModelTester>
#include <QColor>
#include <QLoggingCategory>
#include <QObject>
#include <QPersistentModelIndex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QTest>

#include "../testhelpers.h"
#include "dataitems/qtdid.h"
#include "dataitems/qtditemdatarole.h"
#include "dataitems/treenode.h"
#include "persistedtreeitemmodelstestbase.h"
#include "utils/modeliteration.h"

TestTagItemModels::TestTagItemModels(QObject *parent)
    : PersistedTreeItemModelsTestBase{parent}
{}

void TestTagItemModels::initTestCase() {
    PersistedTreeItemModelsTestBase::initTestCase();
    TestHelpers::assert_table_exists("tags");
}

void TestTagItemModels::init() {
    PersistedTreeItemModelsTestBase::init();
    TestHelpers::setup_item_model(this->model, this->get_db_connection_name());
    TestHelpers::setup_proxy_item_model(this->flat_model, this->model.get());
    // Proxy model will be deleted by base model destructor
}

void TestTagItemModels::cleanup() {
    this->assert_correctness_of_proxy_models();
    this->assert_model_persistence();
    this->model.reset();
    PersistedTreeItemModelsTestBase::cleanup();
}

void TestTagItemModels::test_initial_dataset_represented_correctly() const {
    this->assert_initial_dataset_representation_base_model();
    this->assert_initial_dataset_representation_flat_model();
}

void TestTagItemModels::test_remove_single_row() const {
    auto* base_model = this->model.get();
    const int initial_row_number = ModelIteration::count_model_rows(base_model);
    remove_single_row_without_children(*base_model);
    QCOMPARE(ModelIteration::count_model_rows(base_model), initial_row_number - 1);
    QCOMPARE(ModelIteration::count_model_rows(this->flat_model.get()), initial_row_number - 1);
}

void TestTagItemModels::test_remove_rows_with_children() const {
    auto* base_model = this->model.get();
    TestTagItemModels::remove_children_of_first_top_level_index(*base_model);
    QCOMPARE(ModelIteration::count_model_rows(base_model), 4);
    QCOMPARE(ModelIteration::count_model_rows(this->flat_model.get()), 4);

    const QStringList remaining_expected = {"Private", "Work", "Mails", "Spreadsheet"};
    const auto remaining = TestHelpers::get_display_roles(*base_model);
    const auto remaining_flat = TestHelpers::get_display_roles(*this->flat_model);
    QCOMPARE(remaining, remaining_expected);
    QCOMPARE(remaining, remaining_flat);
}

void TestTagItemModels::test_remove_single_row_with_nested_children() const {
    this->model->removeRow(0, QModelIndex());
    QCOMPARE(ModelIteration::count_model_rows(this->model.get()), 3);
    QCOMPARE(ModelIteration::count_model_rows(this->flat_model.get()), 3);
    QCOMPARE(this->model->rowCount(), 1);
}

void TestTagItemModels::test_create_toplevel_tag() const {
    const int old_row_count = this->model->rowCount();
    const int old_row_count_flat_model = this->flat_model->rowCount();
    const QString new_tag_name = "new tag name";
    QCOMPARE(this->model->create_tag(new_tag_name), true);

    QCOMPARE(this->model->rowCount(), old_row_count + 1);
    QCOMPARE(this->flat_model->rowCount(), old_row_count_flat_model + 1);

    const auto new_tag_index = this->model->index(this->model->rowCount()-1, 0);
    QCOMPARE(new_tag_index.data().toString(), new_tag_name);
    const auto tag1_color = new_tag_index.data(Qt::DecorationRole).value<QColor>();
    QVERIFY(!tag1_color.isValid());
    const auto tag1_uuid = new_tag_index.data(UuidRole).value<TagId>();
    QVERIFY(tag1_uuid.is_valid());
}

void TestTagItemModels::test_create_tag_with_parent() const {
    const int old_row_count = this->model->rowCount();
    const int old_row_count_flat_model = this->flat_model->rowCount();
    const auto vegetable_index = this->model->index(1, 0);
    const int vegetable_row_count = this->model->rowCount(vegetable_index);
    QCOMPARE(vegetable_index.data().toString(), "Work");

    const QString new_tag_name = "new tag name";
    const QColor new_tag_color = QColor::fromString("#1234AB");
    QCOMPARE(this->model->create_tag(new_tag_name, new_tag_color, vegetable_index), true);
    QCOMPARE(this->model->rowCount(), old_row_count);
    QCOMPARE(this->flat_model->rowCount(), old_row_count_flat_model + 1);
    QCOMPARE(this->model->rowCount(vegetable_index), vegetable_row_count + 1);

    const auto new_tag_index = this->model->index(vegetable_row_count, 0, vegetable_index);
    QCOMPARE(new_tag_index.data().toString(), new_tag_name);
    QCOMPARE(new_tag_index.data(Qt::DecorationRole), new_tag_color);
    QCOMPARE(new_tag_index.parent(), vegetable_index);
    const auto new_tag_uuid = new_tag_index.data(UuidRole).value<TagId>();
    QVERIFY(new_tag_uuid.is_valid());

    const auto *new_tag_item = static_cast<TreeNode*>(
        new_tag_index.internalPointer()
    );
    QCOMPARE(
        new_tag_item->get_parent()->get_data(UuidRole),
        vegetable_index.data(UuidRole)
    );
}

void TestTagItemModels::test_data_change_of_toplevel_item() const {
    const auto index = this->model->index(0, 0);

    const auto green = QColor(Qt::green);
    const QString new_display_role = "new name";
    this->model->setData(index, new_display_role, Qt::DisplayRole);
    this->model->setData(index, green, Qt::DecorationRole);
    QCOMPARE(index.data(), new_display_role);
    QCOMPARE(this->model->data(index, Qt::DecorationRole), green);

    const auto flat_proxy_index = this->flat_model->mapFromSource(index);
    QCOMPARE(flat_proxy_index.data(), new_display_role);
    QCOMPARE(flat_proxy_index.data(Qt::DecorationRole), green);
}

void TestTagItemModels::test_data_change_of_child_item() const {
    auto index = this->model->index(2, 0, this->model->index(0, 0));
    const QString new_display_role = "new name";

    auto old_uuid = index.data(UuidRole).value<TagId>();
    auto new_uuid = TagId::create();
    QVERIFY(!this->model->setData(index, new_uuid, UuidRole));
    QCOMPARE(
        index.data(UuidRole).toString(),
        old_uuid.toString()
    );

    QVERIFY(this->model->setData(index, new_display_role, Qt::DisplayRole));
    QCOMPARE(index.data(), new_display_role);
    QCOMPARE(
        this->flat_model->mapFromSource(index).data(),
        new_display_role
    );
}

void TestTagItemModels::assert_initial_dataset_representation_base_model() const {
    QCOMPARE(this->model->rowCount(), 2);
    for (int row=0; row<this->model->rowCount(); row++) {
        auto row_index = this->model->index(row, 0);
        if (row_index.data() == "Private") {
            QCOMPARE(this->model->rowCount(row_index), 4);
        } else if (row_index.data() == "Work") {
            QCOMPARE(this->model->rowCount(row_index), 2);
        } else {
            QFAIL("Unexpected model entry at top level!");
        }
    }
    QCOMPARE(ModelIteration::count_model_rows(this->model.get()), 10);
}

void TestTagItemModels::assert_correct_from_source_mapping_recursively(
    const QModelIndex& source_index,
    int expected_proxy_row
) const {
    const std::function<void(const QModelIndex&)> correct_mapping_assertion_operation
        = [this, &expected_proxy_row](const QModelIndex& source_index) {
        auto proxy_index = this->flat_model->mapFromSource(source_index);
        QCOMPARE(proxy_index.internalPointer(), source_index.internalPointer());
        QCOMPARE(proxy_index.row(), expected_proxy_row++);
    };
    ModelIteration::model_foreach(*this->model, correct_mapping_assertion_operation, source_index);
}

void TestTagItemModels::assert_correct_proxy_mapping() const {
    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto proxy_index = this->flat_model->index(row, 0);
        auto source_index = this->flat_model->mapToSource(proxy_index);
        QCOMPARE(proxy_index.internalPointer(), source_index.internalPointer());
    }

    this->assert_correct_from_source_mapping_recursively(QModelIndex());
}

void TestTagItemModels::assert_initial_dataset_representation_flat_model() const {
    const int total_source_rows = ModelIteration::count_model_rows(this->model.get());
    const int total_rows = ModelIteration::count_model_rows(this->flat_model.get());
    const int top_level_rows = this->flat_model->rowCount();
    QCOMPARE(total_rows, total_source_rows);
    QCOMPARE(top_level_rows, total_source_rows);

    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto row_index = this->flat_model->index(row, 0);
        QCOMPARE(this->flat_model->rowCount(row_index), 0);
    }
}

void TestTagItemModels::assert_correctness_of_proxy_models() const {
    const int model_size = ModelIteration::count_model_rows(this->model.get());
    QCOMPARE(ModelIteration::count_model_rows(this->flat_model.get()), model_size);
    QCOMPARE(this->flat_model->rowCount(), model_size);

    auto tag_names = TestHelpers::get_display_roles(*this->model);
    auto proxy_tag_names = TestHelpers::get_display_roles(*this->flat_model);
    QCOMPARE(tag_names, proxy_tag_names);

    this->assert_correct_proxy_mapping();
}

void TestTagItemModels::assert_model_persistence() const {
    std::unique_ptr<TagItemModel> model_reloaded_from_db;

    TestHelpers::setup_item_model(model_reloaded_from_db, this->get_db_connection_name());

    TestHelpers::assert_model_equality(
        *model_reloaded_from_db,
        *this->model,
        {Qt::DisplayRole, Qt::DecorationRole, UuidRole},
        TestHelpers::compare_indices_by_uuid
    );
}

void TestTagItemModels::remove_single_row_without_children(QAbstractItemModel& model) {
    QModelIndex index_without_child = QModelIndex();
    while (model.hasChildren(index_without_child)) {
        index_without_child = model.index(0, 0, index_without_child);
    }

    model.removeRow(
        index_without_child.row(),
        index_without_child.parent()
    ); // Remove first row without children
}

void TestTagItemModels::remove_children_of_first_top_level_index(QAbstractItemModel& model) {
    auto first_toplevel_index = model.index(0, 0);
    model.removeRows(
        0,
        model.rowCount(first_toplevel_index),
        first_toplevel_index
    ); // Remove multiple rows including children
}

void TestTagItemModels::test_changing_parent_new_parent_can_not_be_descendant() const {
    auto index_ultimate_parent = TestHelpers::find_model_index_by_display_role(*this->model, "Private");
    auto index_grand_child = TestHelpers::find_model_index_by_display_role(*this->model, "Vacation");
    QVERIFY(
        !this->model->change_parent(
            index_ultimate_parent,
            index_grand_child.data(UuidRole).value<TagId>()
        )
    );
}

void TestTagItemModels::test_changing_parent_new_parent_can_not_be_oneself() const {
    auto index = TestHelpers::find_model_index_by_display_role(*this->model, "Fun");
    QVERIFY(
        !this->model->change_parent(
            index,
            index.data(UuidRole).value<TagId>()
        )
    );
}

void TestTagItemModels::test_removing_parent() const {
    auto index = TestHelpers::find_model_index_by_display_role(*this->model, "Vacation");
    auto parent_index = index.parent();
    auto top_level_count = this->model->rowCount();
    auto parent_index_child_count = this->model->rowCount(parent_index);

    QVERIFY(this->model->change_parent(index, TagId()));
    QCOMPARE(this->model->rowCount(), top_level_count + 1);
    QCOMPARE(this->model->rowCount(parent_index), parent_index_child_count - 1);

    auto new_index = TestHelpers::find_model_index_by_display_role(*this->model, "Vacation");
    QVERIFY(!new_index.parent().isValid());
}

void TestTagItemModels::test_changing_parent_to_grand_parent() const {
    auto index_grand_parent = TestHelpers::find_model_index_by_display_role(*this->model, "Private");
    auto index_grand_child = TestHelpers::find_model_index_by_display_role(*this->model, "Vacation");
    auto index_parent = index_grand_child.parent();

    auto grand_parent_child_count = this->model->rowCount(index_grand_parent);
    auto parent_child_count = this->model->rowCount(index_parent);

    QVERIFY(
        this->model->change_parent(
            index_grand_child,
            index_grand_parent.data(UuidRole).value<TagId>()
        )
    );
    QCOMPARE(this->model->rowCount(index_grand_parent), grand_parent_child_count+1);
    QCOMPARE(this->model->rowCount(index_parent), parent_child_count-1);

    auto new_index = TestHelpers::find_model_index_by_display_role(*this->model, "Vacation");
    QCOMPARE(new_index.parent(), index_grand_parent);
}

void TestTagItemModels::test_changing_parent_to_different_subtree() const {
    auto index = QPersistentModelIndex(
        TestHelpers::find_model_index_by_display_role(*this->model, "Fun")
    );
    auto old_parent_index = index.parent();
    auto new_parent_index = TestHelpers::find_model_index_by_display_role(*this->model, "Work");

    auto child_count = this->model->rowCount(index);
    auto old_parent_child_count = this->model->rowCount(old_parent_index);
    auto new_parent_child_count = this->model->rowCount(new_parent_index);

    QVERIFY(this->model->change_parent(index, new_parent_index.data(UuidRole).value<TagId>()));
    QCOMPARE(this->model->rowCount(index), child_count);
    QCOMPARE(this->model->rowCount(old_parent_index), old_parent_child_count - 1);
    QCOMPARE(this->model->rowCount(new_parent_index), new_parent_child_count + 1);
}

QTEST_GUILESS_MAIN(TestTagItemModels)
