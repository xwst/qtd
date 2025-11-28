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

#ifndef MODELITERATION_H
#define MODELITERATION_H

#include <functional>

#include <QAbstractItemModel>
#include <QModelIndex>

namespace ModelIteration {

int count_model_rows(const QAbstractItemModel* model, const QModelIndex &index = QModelIndex());
QModelIndex next_row_index_depth_first(const QAbstractItemModel* model, QModelIndex current_index = QModelIndex());
QModelIndex model_foreach(
    const QAbstractItemModel& model,
    const std::function<bool(const QModelIndex&)>& operation,
    const QModelIndex& parent_index = QModelIndex()
);
void model_foreach(
    const QAbstractItemModel& model,
    const std::function<void(const QModelIndex&)>& operation,
    const QModelIndex& parent_index = QModelIndex()
);
QModelIndex model_find(
    const QAbstractItemModel& model,
    const std::function<bool(const QModelIndex&)>& operation,
    const QModelIndex& parent_index = QModelIndex()
);

/**
 * @brief Apply a function to each index and store the results in a list in depth first order
 *
 * Only the first column of the model is processed, all other columns are ignored.
 * The optional parent_index can be specified as a starting point; only the given index
 * and all its (recursive) children will be visited.
 *
 * @param model the model to traverse
 * @param operation the mapping function to execute
 * @param parent_index the starting point
 */
template <typename T>
QList<T> model_flat_map(
    const QAbstractItemModel& model,
    const std::function<T(const QModelIndex&)>& operation,
    const QModelIndex& parent_index = QModelIndex()
    ) {
    QList<T> result;
    model_foreach(
        model,
        [&result, &operation](const QModelIndex& index) {
            result.push_back(operation(index));
        },
        parent_index
        );
    return result;
}

}

#endif // MODELITERATION_H
