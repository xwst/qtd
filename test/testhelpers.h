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

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <functional>
#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QAbstractItemModelTester>
#include <QString>
#include <QStringList>

#include "../src/app/model/tag.h"

class TestHelpers
{
private:
    static std::vector<QModelIndex> get_sorted_children(
        const QAbstractItemModel& model,
        const QModelIndex& parent,
        const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator
    );
public:

    // Inherit QObject to spy on signals:
    class TestTag : public Tag, public QObject {
    public:
        explicit TestTag(const QString& name) : Tag(name), QObject(nullptr) {}
    };

    static bool setup_database();
    static void assert_table_exists(const QString& table_name);
    static void populate_database();
    static void assert_model_equality(
        const QAbstractItemModel& model_under_test,
        const QAbstractItemModel& model_expectation,
        const QSet<int>& roles_to_check,
        const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator,
        const QModelIndex& index_of_model_under_test = QModelIndex(),
        const QModelIndex& index_of_model_expectation = QModelIndex()
    );
    static void assert_index_equality(
        const QModelIndex& index1,
        const QModelIndex& index2,
        const QSet<int>& roles_to_check
    );
    static bool compare_indices_by_uuid(
        const QModelIndex& index_1,
        const QModelIndex& index_2
    );

    template <typename T, typename... Types>
    static void setup_item_model(
        std::unique_ptr<T>& model,
        Types... constructor_parameters
    ) {
        model = std::make_unique<T>(constructor_parameters...);
        new QAbstractItemModelTester(model.get(), model.get());
        // ModelTester will be deleted by the models destructor.
    }

    template <typename T, typename... Types>
    static void setup_proxy_item_model(
        std::unique_ptr<T>& model,
        QAbstractItemModel* source_model,
        Types... constructor_parameters
    ) {
        model = std::make_unique<T>(constructor_parameters...);
        model->setSourceModel(source_model);
        new QAbstractItemModelTester(model.get(), source_model);
        // ModelTester will be deleted by the models destructor.
    }

    static QStringList get_display_roles(
        const QAbstractItemModel& model,
        const QModelIndex& parent = QModelIndex()
    );

    static QModelIndex find_model_index_by_display_role(
        const QAbstractItemModel& model,
        const QString& display_role,
        const QModelIndex& parent = QModelIndex()
    );
};

#endif // TESTHELPERS_H
