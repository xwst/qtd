#ifndef TESTFILTEREDTASKITEMMODEL_H
#define TESTFILTEREDTASKITEMMODEL_H

#include <QObject>
#include <QTest>

#include "persistedtreeitemmodelstestbase.h"
#include "../../src/app/model/filteredtaskitemmodel.h"
#include "../../src/app/model/taskitemmodel.h"

class TestFilteredTaskItemModel : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<TaskItemModel> base_model;
    std::unique_ptr<FilteredTaskItemModel> model;

public:
    explicit TestFilteredTaskItemModel(QObject *parent = nullptr);

private slots:
    // Test setup/cleanup:
    void initTestCase();

    // Test functions:
    void test_filter_single_word();
    void test_filter_multiple_words();
    void test_filter_with_quotes();
    void test_filter_for_task_details();
    void test_no_matches();
    void test_no_filter();

    void test_filter_independent_of_filter_word_order();
    void test_repeat_words_has_no_effect();

    void test_modifying_base_model_propagates_to_proxy();

    void test_parents_become_childless_if_no_child_matches();
    void test_matching_children_are_kept_if_parents_are_filtered_out();
};

#endif // TESTFILTEREDTASKITEMMODEL_H
