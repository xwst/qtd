#include "testtreeitemmodel.h"

#include <QObject>
#include <QSignalSpy>
#include <QTest>
#include <QVariant>

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
    this->model = std::make_unique<TreeItemModel<TestHelpers::TestTag>>();
    new QAbstractItemModelTester(this->model.get(), this->model.get());

    QVERIFY(this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("A")));
    QVERIFY(this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("B")));
    auto B_index = this->model->index(1, 0);
    QVERIFY(this->model->add_tree_item(
        std::make_unique<TestHelpers::TestTag>("B1"), B_index
    ));
    QCOMPARE(this->model->get_size(), 3);
}

void TestTreeItemModel::test_initial_setup() {
    auto A_index = this->model->index(0, 0);
    auto B_index = this->model->index(1, 0);
    auto B1_index = this->model->index(0, 0, B_index);

    this->verify_item(A_index,  "A",  0, QModelIndex());
    this->verify_item(B_index,  "B",  1, QModelIndex());
    this->verify_item(B1_index, "B1", 0, B_index);
}

void TestTreeItemModel::test_set_data() {
    QVERIFY(!this->model->setData(QModelIndex(), ""));

    auto A_index = this->model->index(0, 0);
    QVERIFY(!this->model->setData(A_index, "new name", Qt::ToolTipRole)); // Unused role
    QCOMPARE(A_index.data(), "A");

    QVERIFY(this->model->setData(A_index, "new name", Qt::DisplayRole));
    QCOMPARE(A_index.data(), "new name");

    auto uuid_str = A_index.data(uuid_role).toString();
    QVERIFY(!this->model->setData(A_index, QUuid::createUuid(), uuid_role));
    QCOMPARE(A_index.data(uuid_role).toString(), uuid_str);
}

void TestTreeItemModel::test_remove_single_row() {
    int initial_size = this->model->get_size();
    auto data = std::make_unique<TestHelpers::TestTag>("about to be deleted");
    auto data_ptr = data.get();
    QSignalSpy spy(data_ptr, SIGNAL(destroyed(QObject*)));
    this->model->add_tree_item(std::move(data));
    this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("C"));

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
    int initial_size = this->model->get_size();
    auto first_new_item  = std::make_unique<TestHelpers::TestTag>("about to be deleted");
    auto second_new_item = std::make_unique<TestHelpers::TestTag>("about to be deleted, too");

    auto first_data_pointer = first_new_item.get();
    auto second_data_pointer = second_new_item.get();

    auto first_spy = QSignalSpy(first_data_pointer, SIGNAL(destroyed(QObject*)));
    auto second_spy = QSignalSpy(second_data_pointer, SIGNAL(destroyed(QObject*)));

    this->model->add_tree_item(std::move(first_new_item));
    this->model->add_tree_item(std::move(second_new_item));

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
    this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("C"));

    QVERIFY(this->model->removeRows(0, 2));
    QCOMPARE(this->model->get_size(), 1);
    QCOMPARE(this->model->rowCount(), 1);
    QCOMPARE(this->model->index(0, 0).data().toString(), "C");
}

void TestTreeItemModel::test_add_mirrored_tree_item() {
    auto B_index = this->model->index(1, 0);
    auto B1_index = this->model->index(0, 0, B_index);
    auto B1_uuid = this->model->index(0, 0, B_index).data(uuid_role).toString();

    QVERIFY(this->model->add_mirrored_tree_item(B1_uuid));
    auto mirror_index = this->model->index(2, 0);
    QCOMPARE(mirror_index.data(uuid_role), B1_uuid);
    QCOMPARE(mirror_index.data(), B1_index.data());
    QCOMPARE(mirror_index.parent(), QModelIndex());

    QSignalSpy spy(this->model.get(), SIGNAL(rowsInserted(const QModelIndex&, int, int)));

    this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("child 1"), B1_index);
    QCOMPARE(this->model->rowCount(B1_index), this->model->rowCount(mirror_index));
    QCOMPARE(this->model->rowCount(B1_index), 1);
    for (auto role : {Qt::DisplayRole, static_cast<Qt::ItemDataRole>(uuid_role)})
        QCOMPARE(
            this->model->index(0, 0, B1_index).data(role),
            this->model->index(0, 0, mirror_index).data(role)
        );
    QCOMPARE(spy.count(), 2);
    QSet<QModelIndex> expected_signalling_indices = {B1_index, mirror_index};
    QCOMPARE(
        QSet({
            spy.takeFirst().at(0).toModelIndex(),
            spy.takeFirst().at(0).toModelIndex()
        }),
        expected_signalling_indices
    );

    this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("child 2"), mirror_index);
    QCOMPARE(this->model->rowCount(B1_index), this->model->rowCount(mirror_index));
    QCOMPARE(this->model->rowCount(B1_index), 2);
    for (auto role : {Qt::DisplayRole, static_cast<Qt::ItemDataRole>(uuid_role)})
        QCOMPARE(this->model->index(1, 0, B1_index).data(role), this->model->index(1, 0, mirror_index).data(role));

    QCOMPARE(spy.count(), 2);
    QCOMPARE(
        QSet({
            spy.takeFirst().at(0).toModelIndex(),
            spy.takeFirst().at(0).toModelIndex()
        }),
        expected_signalling_indices
    );
}

void TestTreeItemModel::test_remove_mirrored_tree_item() {
    auto B_index = this->model->index(1, 0);
    auto B1_index = this->model->index(0, 0, B_index);
    auto B1_uuid = this->model->index(0, 0, B_index).data(uuid_role).toString();

    QVERIFY(this->model->add_mirrored_tree_item(B1_uuid));
    auto mirror_index = this->model->index(2, 0);
    this->model->add_tree_item(std::make_unique<TestHelpers::TestTag>("child 1"), B1_index);

    this->model->removeRows(B1_index.row(), 1, B_index);
    QCOMPARE(this->model->rowCount(), 3);
    QCOMPARE(this->model->get_size(), 3);
    QCOMPARE(this->model->rowCount(mirror_index), 1);
    QCOMPARE(this->model->index(0, 0, mirror_index).data(), "child 1");
}

void TestTreeItemModel::verify_item(
    const QModelIndex& item, QString name, int child_count, const QModelIndex& parent
) {
    QCOMPARE(item.data().toString(), name);
    QCOMPARE(this->model->rowCount(item), child_count);
    QCOMPARE(this->model->columnCount(item), 1);
    QCOMPARE(item.parent(), parent);
    if (parent != QModelIndex()) {
        QCOMPARE(this->model->index(item.row(), 0, parent), item);
    }
}

QTEST_MAIN(TestTreeItemModel)
