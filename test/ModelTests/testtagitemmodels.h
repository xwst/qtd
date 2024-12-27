#ifndef TESTTAGITEMMODELS_H
#define TESTTAGITEMMODELS_H

#include <memory>

#include "../../src/app/model/flatteningproxymodel.h"
#include "../../src/app/model/TagItemModel.h"

class TestTagItemModels : public QObject
{
    Q_OBJECT

public:
    explicit TestTagItemModels(QObject *parent = nullptr);

private:
    std::unique_ptr<TagItemModel> model;
    std::unique_ptr<FlatteningProxyModel> flat_model;

    // Helper functions:
    void assert_initial_dataset_representation_base_model();
    void assert_initial_dataset_representation_flat_model();
    void assert_correct_from_source_mapping_recursively(
        const QModelIndex& source_index = QModelIndex(),
        int expected_proxy_row = -1
    );
    void assert_correct_proxy_mapping();
    void assert_correctness_of_proxy_models();
    void assert_model_persistence();
    static void remove_single_row_without_children(QAbstractItemModel& model);
    static void remove_children_of_first_top_level_index(QAbstractItemModel& model);

private slots:
    // Test setup/cleanup:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test functions:
    void test_initial_dataset_represented_correctly();
    void test_remove_single_row();
    void test_remove_rows_with_children();
    void test_remove_single_row_with_nested_children();
    void test_create_toplevel_tag();
    void test_create_tag_with_parent();
    void test_data_change_of_toplevel_item();
    void test_data_change_of_child_item();
};

#endif // TESTTAGITEMMODELST_H
