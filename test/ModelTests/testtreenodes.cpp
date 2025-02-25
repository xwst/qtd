#include "testtreenodes.h"

#include <QSignalSpy>
#include <QTest>

#include "../testhelpers.h"
#include "../../src/app/model/model_constants.h"

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
    auto initial_child_count = this->root->get_child_count();
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
    auto B = this->root->get_child(1);
    QString name_pattern = "B%1";
    for (int i=2; i<10; i++)
        B->add_child(std::make_unique<TestHelpers::TestTag>(name_pattern.arg(i)));
    QCOMPARE(B->get_child_count(), 9);
    B->remove_children(3, 5);
    QCOMPARE(B->get_child_count(), 4);
    this->verify_item(B->get_child(2), "B3", 0, B);
    this->verify_item(B->get_child(3), "B9", 0, B);
}

void TestTreeNodes::test_remove_child_hierarchy() {
    auto initial_child_count = this->root->get_child_count();
    auto A = this->root->get_child(0);
    auto new_item_data = std::make_unique<TestHelpers::TestTag>("A1");

    auto data_ptr = new_item_data.get();
    QSignalSpy spy(data_ptr, SIGNAL(destroyed(QObject*)));
    A->add_child(std::move(new_item_data));

    QCOMPARE(A->get_child_count(), 1);

    this->root->remove_children(0, 1);
    QCOMPARE(this->root->get_child_count(), initial_child_count-1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().at(0).value<QObject*>(), data_ptr);
}

void TestTreeNodes::test_cloning() {
    auto root_child_count = this->root->get_child_count();
    this->root->add_child(std::make_unique<TestHelpers::TestTag>("C"));
    auto B = this->root->get_child(1);
    auto B_child_count = B->get_child_count();
    auto C = this->root->get_child(2);

    B->add_child(TreeNode::clone(C));
    auto C_clone = B->get_child(B_child_count);

    QCOMPARE(this->root->get_child_count(), root_child_count+1);
    QCOMPARE(B->get_child_count(), B_child_count+1);
    QCOMPARE(C_clone->get_child_count(), C->get_child_count());
    QCOMPARE(C_clone->get_child_count(), 0);
    QCOMPARE(C->get_parent(), this->root.get());
    QCOMPARE(C_clone->get_parent(), B);
    QCOMPARE(C_clone->get_data(Qt::DisplayRole), "C");
}

void TestTreeNodes::test_set_data() {
    auto A = this->root->get_child(0);
    auto initial_name = A->get_data(Qt::DisplayRole);
    auto uuid = A->get_data(uuid_role);

    A->set_data("new name", Qt::ToolTipRole); // Unused role
    QCOMPARE(A->get_data(Qt::DisplayRole), initial_name);
    QCOMPARE(A->get_data(uuid_role), uuid);

    A->set_data("new name", Qt::DisplayRole);
    QCOMPARE(A->get_data(Qt::DisplayRole), "new name");
    QCOMPARE(A->get_data(uuid_role), uuid);

    A->set_data(QUuid::createUuid(), uuid_role);
    QCOMPARE(A->get_data(Qt::DisplayRole), "new name");
    QCOMPARE(A->get_data(uuid_role), uuid);
}

void TestTreeNodes::setup_dummies() {
    this->root = TreeNode::create(std::make_unique<UniqueDataItem>());
    this->root->add_child(std::make_unique<TestHelpers::TestTag>("A"));
    this->root->add_child(TreeNode::create(std::make_unique<TestHelpers::TestTag>("B")));
    this->root->get_child(1)->add_child(std::make_unique<TestHelpers::TestTag>("B1"));
}

void TestTreeNodes::verify_item(TreeNode* item, QString name, int child_count, TreeNode* parent) {
    QCOMPARE(item->get_data(Qt::DisplayRole).toString(), name);
    QCOMPARE(item->get_child_count(), child_count);
    QCOMPARE(item->get_parent(), parent);
    if (parent != nullptr) {
        QCOMPARE(parent->get_child(item->get_row_in_parent()), item);
    }
}

void TestTreeNodes::verify_dummies() {
    this->verify_item(this->root.get(), "", 2, nullptr);
    this->verify_item(this->root->get_child(0), "A", 0, this->root.get());

    auto B = this->root->get_child(1);
    this->verify_item(B, "B", 1, this->root.get());
    this->verify_item(B->get_child(0), "B1", 0, B);
}

QTEST_MAIN(TestTreeNodes)
