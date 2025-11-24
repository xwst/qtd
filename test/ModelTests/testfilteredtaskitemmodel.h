/**
 * Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
 *
 * This file is part of qtd.
 *
 * qtd is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * qtd is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * qtd. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TESTFILTEREDTASKITEMMODEL_H
#define TESTFILTEREDTASKITEMMODEL_H

#include <memory>

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "models/filteredtaskitemmodel.h"
#include "models/taskitemmodel.h"

class TestFilteredTaskItemModel : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<TaskItemModel> base_model;
    std::unique_ptr<FilteredTaskItemModel> model;
    std::unique_ptr<QSignalSpy> spy;

public:
    explicit TestFilteredTaskItemModel(QObject *parent = nullptr);

private slots:
    // Test setup/cleanup:
    static void initTestCase();
    void init();

    // Test functions:
    void test_filter_single_word() const;
    void test_filter_multiple_words() const;
    void test_filter_with_quotes() const;
    void test_filter_for_task_details() const;
    void test_no_search_string_matches() const;
    void test_no_filter() const;

    void test_filter_independent_of_filter_word_order() const;
    void test_repeating_words_has_no_effect() const;

    void test_modifying_base_model_propagates_to_proxy() const;
    void test_adding_children_to_cloned_items_in_base_model() const;

    void test_parents_become_childless_if_no_child_matches() const;
    void test_matching_children_are_kept_if_parents_are_filtered_out() const;

    void test_filter_by_tag_selection() const;
    void test_filter_by_tag_and_search_string() const;
};

#endif // TESTFILTEREDTASKITEMMODEL_H
