#ifndef TESTTREENODES_H
#define TESTTREENODES_H

#include <memory>

#include <QObject>

#include "../../src/app/model/treenode.h"

class TestTreeNodes : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeNodes(QObject *parent = nullptr);

private:
    std::unique_ptr<TreeNode> root;

    void setup_dummies();
    void verify_item(TreeNode* item, QString name, int child_count, TreeNode* parent);
    void verify_dummies();

private slots:
    // Test setup/cleanup:
    void init();
    void cleanup();

    // Test functions:
    void test_add_child();
    void test_remove_single_child();
    void test_remove_multiple_children();
    void test_remove_child_hierarchy();
    void test_cloning();
    void test_set_data();
};

#endif // TESTTREENODES_H
