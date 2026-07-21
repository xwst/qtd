/**
 * Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include "taskadapter.h"

namespace
{

bool is_index_within_range(
    const QModelIndex& index,
    const QModelIndex& top_left,
    const QModelIndex& bottom_right
)
{
    return index.isValid()
        && index.parent() == top_left.parent()
        && index.parent() == top_left.parent()
        && top_left.row() <= index.row()
        && bottom_right.row() >= index.row();
}

} // anonymous namespace

bool TaskAdapter::index_is_new_and_valid(const QModelIndex& idx)
{
    return this->index != idx
        && this->model != nullptr
        && this->model->checkIndex(idx);
}

QVariant TaskAdapter::data(int role) const
{
    if (this->model != nullptr && this->index.isValid())
    {
        return this->model->data(this->index, role);
    }
    return {};
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void TaskAdapter::emit_all_changed()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define X(type, name, role) \
        emit name##Changed();
    #include "backend/dataitems/task_roles.inc"
    #undef X
}

QAbstractItemModel* TaskAdapter::get_model() const
{
    return this->model;
}

QVariant TaskAdapter::get_index() const
{
    return QVariant::fromValue(this->index);
}

void TaskAdapter::set_model(QAbstractItemModel* new_model)
{
    if (new_model == this->model)
    {
        return;
    }
    if (this->model != nullptr)
    {
        QObject::disconnect(this->model, nullptr, this, nullptr);
    }

    this->model = new_model;
    emit this->modelChanged();

    if (new_model != nullptr)
    {
        QObject::connect(
            new_model, &QAbstractItemModel::dataChanged,
            this, &TaskAdapter::on_data_changed
        );
        QObject::connect(
            new_model, &QAbstractItemModel::modelReset,
            this, &TaskAdapter::emit_all_changed
        );
    }
}

void TaskAdapter::set_index(const QVariant& value)
{
    const QModelIndex idx = value.toModelIndex();
    if (this->index_is_new_and_valid(idx))
    {
        this->index = idx;

        emit this->indexChanged();
        this->emit_all_changed();
    }
}

void TaskAdapter::on_data_changed(
    const QModelIndex& top_left,
    const QModelIndex& bottom_right,
    const QList<int>& roles
)
{
    if (is_index_within_range(this->index, top_left, bottom_right))
    {
        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
        #define X(type, name, role) \
            if (roles.isEmpty() || roles.contains(role)) { \
                emit name##Changed(); \
            }
        #include "backend/dataitems/task_roles.inc"
        #undef X
    }
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define X(type, name, role) \
type TaskAdapter::get_##name() const { \
    return this->data(role).value<type>(); \
} \
\
void TaskAdapter::set_##name(const type& value) { \
    if ( \
             this->model != nullptr \
         && this->index.isValid() \
         && value != this->get_##name() \
    ) { \
        this->model->setData( \
            this->index, \
            QVariant::fromValue(value), \
            role \
        ); \
    } \
}

#include "backend/dataitems/task_roles.inc"

#undef X
