#ifndef TESTTREEITEMMODEL_H
#define TESTTREEITEMMODEL_H

#include <QObject>
#include <QSignalSpy>

#include "../testhelpers.h"
#include "testmodelwrappers.h"

class TestTreeItemModel : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeItemModel(QObject *parent = nullptr);

private:
    std::unique_ptr<TreeItemModelTestWrapper> model;

    void setup_initial_model();
    void verify_item(
        const QModelIndex& item, QString name, int child_count, const QModelIndex& parent
    );
    QSet<QModelIndex> model_indices_of_row_change_signals(const QSignalSpy& spy);

private slots:
    // Test setup/cleanup:
    void init();
    void cleanup();

    // Tests:
    void test_initial_setup();
    void test_set_data();
    void test_remove_single_row();
    void test_remove_multiple_rows();
    void test_remove_rows_with_children();
    void test_clone_tree_node();
    void test_remove_clone();
    void test_remove_child_of_clone();
};

#endif // TESTTREEITEMMODEL_H
