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

#include "testhelpers.h"

#include <algorithm>
#include <functional>
#include <vector>

#include <QAbstractItemModel>
#include <QFile>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QTest>
#include <QTextStream>

#include "../src/app/model/model_constants.h"
#include "../src/app/util.h"

namespace {

void assert_index_data_and_dimension_equality(
    const QAbstractItemModel& model_under_test,
    const QAbstractItemModel& model_expectation,
    const QSet<int>& roles_to_check,
    const QModelIndex& index_of_model_under_test,
    const QModelIndex& index_of_model_expectation
) {
    // Index data equality:
    QCOMPARE(
        index_of_model_under_test.isValid(),
        index_of_model_expectation.isValid()
    );
    for (const auto item_data_role : roles_to_check) {
        QCOMPARE(
            index_of_model_under_test.data(item_data_role),
            index_of_model_expectation.data(item_data_role)
        );
    }

    // Dimension equality:
    QCOMPARE(
        model_under_test.rowCount(index_of_model_under_test),
        model_expectation.rowCount(index_of_model_expectation)
    );
    QCOMPARE(
        model_under_test.columnCount(index_of_model_under_test),
        model_expectation.columnCount(index_of_model_expectation)
    );
}

} // anonymous namespace

bool TestHelpers::setup_database() {
    QSqlDatabase database;
    if (QSqlDatabase::contains()) {
        database = QSqlDatabase::database();
        database.close();
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(":memory:");
    }
    database.open();
    QSqlQuery(database).exec("PRAGMA foreign_keys = ON;");
    return Util::create_tables_if_not_exist(database.connectionName());
}

void TestHelpers::assert_table_exists(const QString& table_name) {
    QSqlQuery query;
    const bool result = query.exec("SELECT COUNT(*) FROM " + table_name);
    const QString error_msg = QString("The '%1'-table was not created!").arg(table_name);
    QVERIFY2(result, qPrintable(error_msg));
}

void TestHelpers::populate_database() {
    QFile file(":/resources/sql/generic/populate_database.sql");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in_stream(&file);
    const QString all_queries_str = in_stream.readAll();

    QSqlQuery query;
    for (const QString& query_str : Util::split_queries(all_queries_str)) {
        QVERIFY2(
            query.exec(query_str),
            qPrintable("Error while populating test database: " + query.lastError().text())
        );
    }
}

std::vector<QModelIndex> TestHelpers::get_sorted_children(
    const QAbstractItemModel& model,
    const QModelIndex& parent,
    const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator
) {
    std::vector<QModelIndex> result;
    const int number_of_children = model.rowCount(parent);
    const int column = parent.isValid() ? parent.column() : 0;

    result.reserve(number_of_children);
    for (int row=0; row<number_of_children; row++) {
        result.push_back(model.index(row, column, parent));
    }
    std::sort(result.begin(), result.end(), item_sort_comparator);

    return result;
}

//NOLINTBEGIN(misc-no-recursion)
void TestHelpers::assert_model_equality(
    const QAbstractItemModel& model_under_test,
    const QAbstractItemModel& model_expectation,
    const QSet<int>& roles_to_check,
    const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator,
    const QModelIndex& index_of_model_under_test,
    const QModelIndex& index_of_model_expectation
) {
    // Sanity checks:
    if (
        (&model_under_test == &model_expectation)
        && (index_of_model_under_test == index_of_model_expectation)
    ) {
        QFAIL("Sanity check failed: Models or indices should be different!");
    }

    QVERIFY(model_expectation.checkIndex(index_of_model_expectation));
    QVERIFY(model_under_test.checkIndex(index_of_model_under_test));

    assert_index_data_and_dimension_equality(
        model_under_test,
        model_expectation,
        roles_to_check,
        index_of_model_under_test,
        index_of_model_expectation
    );

    // Sort children by passed comparator:
    const auto children_under_test = TestHelpers::get_sorted_children(
        model_under_test, index_of_model_under_test, item_sort_comparator
    );
    const auto children_expectation = TestHelpers::get_sorted_children(
        model_expectation, index_of_model_expectation, item_sort_comparator
    );

    // Children equality (recursion):
    for (decltype(children_under_test.size()) i=0; i<children_under_test.size(); i++) {
        TestHelpers::assert_model_equality(
            model_under_test,
            model_expectation,
            roles_to_check,
            item_sort_comparator,
            children_under_test[i],
            children_expectation[i]
        );
    }
}
//NOLINTEND(misc-no-recursion)

void TestHelpers::assert_index_equality(
    const QModelIndex& index1,
    const QModelIndex& index2,
    const QSet<int>& roles_to_check
) {
    const int index1_row_count = index1.model()->rowCount(index1);
    QCOMPARE(index1_row_count, index2.model()->rowCount(index2));
    for (int row=0; row<index1_row_count; row++) {
        for (const auto role : roles_to_check) {
            QCOMPARE(
                index1.model()->index(row, index1.column(), index1).data(role),
                index2.model()->index(row, index2.column(), index2).data(role)
            );
        }
    }
}

/**
 * @brief Get a list of all display roles of the first column in the model
 * @param model the model to traverse
 * @param parent specifies the starting point if only a subtree shall be traversed
 * @return a list of strings of the display roles
 */
QStringList TestHelpers::get_display_roles(
    const QAbstractItemModel& model,
    const QModelIndex& parent
) {
    const std::function<QString(const QModelIndex&)> get_display_role
        = [](const QModelIndex& index) -> QString { return index.data().toString(); };
    return Util::model_flat_map(model, get_display_role, parent);
}

/**
 * @brief Use a depth-first-search to find a ModelIndex with a given DisplayRole.
 * @param model the model to traverse
 * @param display_role the string to look for in the display roles of the indices
 * @param parent a model index specifying the subtree to traverse
 * @return the corresponding model index or an invalid model index if no match was found.
 */
QModelIndex TestHelpers::find_model_index_by_display_role(
    const QAbstractItemModel& model,
    const QString& display_role,
    const QModelIndex& parent
) {
    return Util::model_find(
        model,
        [&display_role](const QModelIndex& index){
            return index.data().toString() == display_role;
        },
        parent
    );
}

bool TestHelpers::compare_indices_by_uuid(
    const QModelIndex& index_1,
    const QModelIndex& index_2
) {
    return index_1.data(uuid_role).toUuid() < index_2.data(uuid_role).toUuid();
}
