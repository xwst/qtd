#include "testtagitemmodels.h"

#include <QAbstractItemModelTester>
#include <QLoggingCategory>
#include <QSqlDataBase>
#include <QSqlQuery>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/util.h"

TestTagItemModels::TestTagItemModels(QObject *parent)
    : QObject{parent}
{}

void TestTagItemModels::initTestCase() {

    QLoggingCategory::setFilterRules("qt.modeltest.debug=true");

    TestHelpers::setup_database();
    TestHelpers::assert_table_exists("tags");
}

void TestTagItemModels::cleanupTestCase() {
    QSqlDatabase::database().close();
}

void TestTagItemModels::init() {
    TestHelpers::populate_database();
    QString connection_name = QSqlDatabase::database().connectionName();

    TestHelpers::setup_item_model(this->model, connection_name);
    TestHelpers::setup_proxy_item_model(this->flat_model, this->model.get());
    // Proxy model will be deleted by base model destructor
}

void TestTagItemModels::cleanup() {
    this->assert_correctness_of_proxy_models();

    QSqlQuery query;
    query.exec("DELETE FROM tags");
    query.exec("SELECT COUNT(*) FROM tags");
    query.first();
    QCOMPARE(query.value(0), 0);
    this->model.reset();
}

void TestTagItemModels::test_initial_dataset_represented_correctly() {
    this->assert_initial_dataset_representation_base_model();
    this->assert_initial_dataset_representation_flat_model();
}

void TestTagItemModels::test_remove_single_row() {
    auto base_model = this->model.get();
    int initial_row_number = Util::count_model_rows(base_model);
    remove_single_row_without_children(*base_model);
    QCOMPARE(Util::count_model_rows(base_model), initial_row_number - 1);
    QCOMPARE(Util::count_model_rows(this->flat_model.get()), initial_row_number - 1);
}

void TestTagItemModels::test_remove_rows_with_children() {
    auto base_model = this->model.get();
    this->remove_children_of_first_top_level_index(*base_model);
    QCOMPARE(Util::count_model_rows(base_model), 4);
    QCOMPARE(Util::count_model_rows(this->flat_model.get()), 4);

    QStringList remaining_expected = {"Fruits", "Vegetables", "Carrots", "Chickpeas"};
    auto remaining = TestHelpers::get_display_roles(*base_model);
    auto remaining_flat = TestHelpers::get_display_roles(*this->flat_model.get());
    QCOMPARE(remaining, remaining_expected);
    QCOMPARE(remaining, remaining_flat);
}

void TestTagItemModels::test_remove_single_row_with_nested_children() {
    this->model.get()->removeRow(0, QModelIndex());
    QCOMPARE(Util::count_model_rows(this->model.get()), 3);
    QCOMPARE(Util::count_model_rows(this->flat_model.get()), 3);
    QCOMPARE(this->model->rowCount(), 1);
}

void TestTagItemModels::test_create_toplevel_tag() {
    int old_row_count = this->model->rowCount();
    int old_row_count_flat_model = this->flat_model->rowCount();
    QString new_tag_name = "new tag name";
    QCOMPARE(this->model->create_tag(new_tag_name), true);

    QCOMPARE(this->model->rowCount(), old_row_count + 1);
    QCOMPARE(this->flat_model->rowCount(), old_row_count_flat_model + 1);

    auto new_tag_index = this->model->index(this->model->rowCount()-1, 0);
    QCOMPARE(new_tag_index.data().toString(), new_tag_name);
    QColor tag1_color = new_tag_index.data(Qt::DecorationRole).value<QColor>();
    QVERIFY(!tag1_color.isValid());
    auto tag1_uuid_str = new_tag_index.data(TagItemModel::uuid_role).toString();
    QVERIFY(!QUuid::fromString(tag1_uuid_str).isNull());
}

void TestTagItemModels::test_create_tag_with_parent() {
    int old_row_count = this->model->rowCount();
    int old_row_count_flat_model = this->flat_model->rowCount();
    auto vegetable_index = this->model->index(1, 0);
    int vegetable_row_count = this->model->rowCount(vegetable_index);
    QCOMPARE(vegetable_index.data().toString(), "Vegetables");

    QString new_tag_name = "new tag name";
    QColor new_tag_color = QColor::fromString("#1234AB");
    QCOMPARE(this->model->create_tag(new_tag_name, new_tag_color, vegetable_index), true);
    QCOMPARE(this->model->rowCount(), old_row_count);
    QCOMPARE(this->flat_model->rowCount(), old_row_count_flat_model + 1);
    QCOMPARE(this->model->rowCount(vegetable_index), vegetable_row_count + 1);

    auto new_tag_index = this->model->index(vegetable_row_count, 0, vegetable_index);
    QCOMPARE(new_tag_index.data().toString(), new_tag_name);
    QCOMPARE(new_tag_index.data(Qt::DecorationRole), new_tag_color);
    QCOMPARE(new_tag_index.parent(), vegetable_index);
    auto new_tag_uuid = QUuid::fromString(new_tag_index.data(TagItemModel::uuid_role).toString());
    QVERIFY(!new_tag_uuid.isNull());

    auto new_tag = static_cast<Tag*>(new_tag_index.internalPointer());
    auto vegetable_uuid_str = vegetable_index.data(TagItemModel::uuid_role).toString();
    QCOMPARE(new_tag->get_parent()->get_uuid_string(), vegetable_uuid_str);
}

void TestTagItemModels::test_data_change_of_toplevel_item() {
    auto index = this->model->index(0, 0);

    auto green = QColor(Qt::green);
    QString new_display_role = "new name";
    this->model->setData(index, new_display_role, Qt::DisplayRole);
    this->model->setData(index, green, Qt::DecorationRole);
    QCOMPARE(index.data(), new_display_role);
    QCOMPARE(this->model->data(index, Qt::DecorationRole), green);

    auto flat_proxy_index = this->flat_model->mapFromSource(index);
    QCOMPARE(flat_proxy_index.data(), new_display_role);
    QCOMPARE(flat_proxy_index.data(Qt::DecorationRole), green);
}

void TestTagItemModels::test_data_change_of_child_item() {
    auto index = this->model->index(2, 0, this->model->index(0, 0));
    QString new_display_role = "new name";

    auto old_uuid = index.data(TagItemModel::uuid_role).toUuid();
    auto new_uuid = QUuid::createUuid();
    QVERIFY(!this->model->setData(index, new_uuid, TagItemModel::uuid_role));
    QCOMPARE(
        index.data(TagItemModel::uuid_role).toString(),
        old_uuid.toString(QUuid::WithoutBraces)
    );

    QVERIFY(this->model->setData(index, new_display_role, Qt::DisplayRole));
    QCOMPARE(index.data(), new_display_role);
    QCOMPARE(
        this->flat_model->mapFromSource(index).data(),
        new_display_role
    );
}

void TestTagItemModels::assert_initial_dataset_representation_base_model() {
    QCOMPARE(this->model->rowCount(), 2);
    for (int row=0; row<this->model->rowCount(); row++) {
        auto row_index = this->model->index(row, 0);
        if (row_index.data() == "Fruits")
            QCOMPARE(this->model->rowCount(row_index), 4);
        else if (row_index.data() == "Vegetables")
            QCOMPARE(this->model->rowCount(row_index), 2);
        else QFAIL("Unexpected model entry at top level!");
    }
    QCOMPARE(Util::count_model_rows(this->model.get()), 10);
}

void TestTagItemModels::assert_correct_from_source_mapping_recursively(
    const QModelIndex& source_index,
    int expected_proxy_row
) {
    auto proxy_index = this->flat_model->mapFromSource(source_index);
    QCOMPARE(proxy_index.internalPointer(), source_index.internalPointer());
    QCOMPARE(proxy_index.row(), expected_proxy_row);

    for (int row=0; row<this->model->rowCount(source_index); row++) {
        auto child_index = this->model->index(row, 0, source_index);
        this->assert_correct_from_source_mapping_recursively(child_index, expected_proxy_row+1);
        expected_proxy_row += Util::count_model_rows(this->model.get(), child_index);
    }
}

void TestTagItemModels::assert_correct_proxy_mapping() {
    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto proxy_index = this->flat_model->index(row, 0);
        auto source_index = this->flat_model->mapToSource(proxy_index);
        QCOMPARE(proxy_index.internalPointer(), source_index.internalPointer());
    }

    this->assert_correct_from_source_mapping_recursively(QModelIndex());
}

void TestTagItemModels::assert_initial_dataset_representation_flat_model() {
    int total_source_rows = Util::count_model_rows(this->model.get());
    int total_rows = Util::count_model_rows(this->flat_model.get());
    int top_level_rows = this->flat_model->rowCount();
    QCOMPARE(total_rows, total_source_rows);
    QCOMPARE(top_level_rows, total_source_rows);

    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto row_index = this->flat_model->index(row, 0);
        QCOMPARE(this->flat_model->rowCount(row_index), 0);
    }
}

void TestTagItemModels::assert_correctness_of_proxy_models() {
    int model_size = Util::count_model_rows(this->model.get());
    QCOMPARE(Util::count_model_rows(this->flat_model.get()), model_size);
    QCOMPARE(this->flat_model->rowCount(), model_size);

    auto tag_names = TestHelpers::get_display_roles(*this->model.get());
    auto proxy_tag_names = TestHelpers::get_display_roles(*this->flat_model.get());
    QCOMPARE(tag_names, proxy_tag_names);

    this->assert_correct_proxy_mapping();
}

void TestTagItemModels::remove_single_row_without_children(QAbstractItemModel& model) {
    QModelIndex index_without_child = QModelIndex();
    while (model.hasChildren(index_without_child))
        index_without_child = model.index(0, 0, index_without_child);

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

QTEST_MAIN(TestTagItemModels)
