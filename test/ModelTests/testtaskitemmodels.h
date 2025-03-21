#ifndef TESTTASKITEMMODEL_H
#define TESTTASKITEMMODEL_H

#include <memory>

#include <QObject>

#include "persistedtreeitemmodelstestbase.h"
#include "../../src/app/model/task.h"
#include "../../src/app/model/taskitemmodel.h"

class TestTaskItemModel : public PersistedTreeItemModelsTestBase
{
    Q_OBJECT

private:
    std::unique_ptr<TaskItemModel> model;

    // Helper functions:
    void assert_initial_dataset_representation_base_model();
    void assert_model_persistence();
    void assert_index_equality(
        const QModelIndex& index1,
        const QModelIndex& index2
    );
    void find_task_by_title_and_assert_correctness_of_data(
        QString title,
        Task::Status status,
        QDateTime start_datetime,
        QDateTime due_datetime,
        int number_of_children
    );

public:
    explicit TestTaskItemModel(QObject *parent = nullptr);

private slots:
    // Test setup/cleanup:
    void initTestCase();
    void init();
    void cleanup();

    // Test functions:
    void test_initial_dataset_represented_correctly();
    void test_model_stores_text_documents();
    void test_data_change_of_unique_task();
    void test_data_change_of_cloned_task();
    void test_remove_rows();
    void test_create_task();
    void test_add_dependency();
    void test_can_not_create_dependency_cycle();

};

#endif // TESTTASKITEMMODEL_H
