#ifndef TESTTREEITEMS_H
#define TESTTREEITEMS_H

#include <memory>

#include <QObject>

#include "../../src/app/model/treeitem.h"

class TestTreeItems : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeItems(QObject *parent = nullptr);

private:
    std::unique_ptr<TreeItem> root;

    void setup_dummies();
    void verify_item(TreeItem* item, QString name, int child_count, TreeItem* parent);
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
    void test_mirroring();
    void test_set_data();
};

#endif // TESTTREEITEMS_H
