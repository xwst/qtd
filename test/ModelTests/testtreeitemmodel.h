#ifndef TESTTREEITEMMODEL_H
#define TESTTREEITEMMODEL_H

#include <QObject>
#include <QSignalSpy>

#include "../testhelpers.h"
#include "../../src/app/model/treeitemmodel.h"

class TestTreeItemModel : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeItemModel(QObject *parent = nullptr);

private:
    std::unique_ptr<TreeItemModel<TestHelpers::TestTag>> model;

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
    void test_add_mirrored_tree_item();
    void test_remove_mirrored_tree_item();
    void test_remove_chiled_of_mirrored_tree_item();
};

#endif // TESTTREEITEMMODEL_H
