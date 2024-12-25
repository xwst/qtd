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


void TestTagItemModels::test_proxy_mapping() {
    for (int row=0; row<this->flat_model->rowCount(); row++) {
        auto proxy_index = this->flat_model->index(row, 0);
        auto source_index = this->flat_model->mapToSource(proxy_index);
        QCOMPARE(proxy_index.internalPointer(), source_index.internalPointer());
    }

    this->assert_correct_from_source_mapping_recursively(QModelIndex());
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

    QSet<QString> remaining_expected = {"Fruits", "Vegetables", "Carrots", "Chickpeas"};
    auto remaining = TestHelpers::get_display_roles(*base_model);
    auto remaining_flat = TestHelpers::get_display_roles(*this->flat_model.get());
    QCOMPARE(remaining, remaining_expected);
    QCOMPARE(remaining, remaining_flat);
}

void TestTagItemModels::test_remove_single_row_with_nested_children() {
    this->model.get()->removeRow(0, QModelIndex());
    QCOMPARE(Util::count_model_rows(this->model.get()), 3);
    QCOMPARE(this->model->rowCount(), 1);
}

void TestTagItemModels::test_create_tag() {
    int old_row_count = this->model->rowCount();
    int old_row_count_flat_model = this->flat_model->rowCount();
    QString tag1_name = "new tag #1";
    QCOMPARE(this->model->create_tag(tag1_name), true);

    QCOMPARE(this->model->rowCount(), old_row_count + 1);
    QCOMPARE(this->flat_model->rowCount(), old_row_count_flat_model + 1);

    auto tag1_index = this->model->index(this->model->rowCount()-1, 0);
    QCOMPARE(tag1_index.data().toString(), tag1_name);
    QColor tag1_color = tag1_index.data(Qt::DecorationRole).value<QColor>();
    QVERIFY(!tag1_color.isValid());
    auto tag1_uuid_str = tag1_index.data(TagItemModel::uuid_role).toString();
    QVERIFY(!QUuid::fromString(tag1_uuid_str).isNull());

    auto vegetable_index = this->model->index(1, 0);
    int vegetable_row_count = this->model->rowCount(vegetable_index);
    QCOMPARE(vegetable_index.data().toString(), "Vegetables");

    QString tag2_name = "new tag #2";
    QColor tag2_color = QColor::fromString("#1234AB");
    QCOMPARE(this->model->create_tag(tag2_name, tag2_color, vegetable_index), true);
    QCOMPARE(this->model->rowCount(), old_row_count + 1);
    QCOMPARE(this->flat_model->rowCount(), old_row_count_flat_model + 2);
    QCOMPARE(this->model->rowCount(vegetable_index), vegetable_row_count + 1);

    auto tag2_index = this->model->index(vegetable_row_count, 0, vegetable_index);
    QCOMPARE(tag2_index.data().toString(), tag2_name);
    QCOMPARE(tag2_index.data(Qt::DecorationRole), tag2_color);
    QCOMPARE(tag2_index.parent(), vegetable_index);
    auto tag2_uuid = QUuid::fromString(tag2_index.data(TagItemModel::uuid_role).toString());
    QVERIFY(!tag2_uuid.isNull());

    auto tag2 = static_cast<Tag*>(tag2_index.internalPointer());
    auto vegetable_uuid_str = vegetable_index.data(TagItemModel::uuid_role).toString();
    QCOMPARE(tag2->get_parent()->get_uuid_string(), vegetable_uuid_str);
}

void TestTagItemModels::test_data_change() {
    auto top_level_index = this->model->index(0, 0);
    auto child_index = this->model->index(2, 0, top_level_index);

    auto green = QColor(Qt::green);
    QString new_display_role = "new name";
    this->model->setData(top_level_index, new_display_role, Qt::DisplayRole);
    this->model->setData(top_level_index, green, Qt::DecorationRole);
    QCOMPARE(top_level_index.data(), new_display_role);
    QCOMPARE(this->model->data(top_level_index, Qt::DecorationRole), green);

    auto flat_top_level_index = this->flat_model->mapFromSource(top_level_index);
    QCOMPARE(flat_top_level_index.data(), new_display_role);
    QCOMPARE(flat_top_level_index.data(Qt::DecorationRole), green);

    auto old_uuid = child_index.data(TagItemModel::uuid_role).toUuid();
    auto new_uuid = QUuid::createUuid();
    QVERIFY(!this->model->setData(child_index, new_uuid, TagItemModel::uuid_role));
    QCOMPARE(
        child_index.data(TagItemModel::uuid_role).toString(),
        old_uuid.toString(QUuid::WithoutBraces)
    );

    QVERIFY(this->model->setData(child_index, new_display_role, Qt::DisplayRole));
    QCOMPARE(child_index.data(), new_display_role);
    QCOMPARE(
        this->flat_model->mapFromSource(child_index).data(),
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
