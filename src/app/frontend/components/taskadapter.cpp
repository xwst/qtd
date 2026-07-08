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

namespace {

static bool is_index_within_range(
    const QModelIndex& index,
    const QModelIndex& top_left,
    const QModelIndex& bottom_right
) {
    return index.isValid()
        && index.parent() == top_left.parent()
        && index.parent() == top_left.parent()
        && top_left.row() <= index.row()
        && bottom_right.row() >= index.row();
}

} // anonymous namespace

bool TaskAdapter::index_is_new_and_valid(const QModelIndex& idx) {
    return this->m_index != idx
        && this->m_model != nullptr
        && this->m_model->checkIndex(idx);
}

QVariant TaskAdapter::data(int role) const {
    if (this->m_model != nullptr && this->m_index.isValid()) {
        return this->m_model->data(this->m_index, role);
    }
    return {};
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void TaskAdapter::emit_all_changed() {
    #define X(type, name, role) \
        emit name##Changed();
    #include "backend/dataitems/task_roles.inc"
    #undef X
}

QAbstractItemModel* TaskAdapter::get_model() const {
    return this->m_model;
}

QVariant TaskAdapter::get_index() const {
    return QVariant::fromValue(this->m_index);
}

void TaskAdapter::set_model(QAbstractItemModel* model) {
    if (model == this->m_model) {
        return;
    }
    if (this->m_model != nullptr) {
        QObject::disconnect(this->m_model, nullptr, this, nullptr);
    }

    this->m_model = model;
    emit this->modelChanged();

    if (model != nullptr) {
        QObject::connect(
            this->m_model, &QAbstractItemModel::dataChanged,
            this, &TaskAdapter::on_data_changed
        );
        QObject::connect(
            this->m_model, &QAbstractItemModel::modelReset,
            this, &TaskAdapter::emit_all_changed
        );
    }
}

void TaskAdapter::set_index(const QVariant& value) {
    const QModelIndex idx = value.toModelIndex();
    if (this->index_is_new_and_valid(idx)) {
        this->m_index = idx;

        emit this->indexChanged();
        this->emit_all_changed();
    }
}

void TaskAdapter::on_data_changed(
    const QModelIndex& top_left,
    const QModelIndex& bottom_right,
    const QList<int>& roles
) {
    if (is_index_within_range(this->m_index, top_left, bottom_right)) {
        #define X(type, name, role) \
            if (roles.isEmpty() || roles.contains(role)) { \
                emit name##Changed(); \
            }
        #include "backend/dataitems/task_roles.inc"
        #undef X
    }
}

#define X(type, name, role) \
type TaskAdapter::get_##name() const { \
    return this->data(role).value<type>(); \
} \
\
void TaskAdapter::set_##name(const type& value) { \
    if ( \
             this->m_model != nullptr \
         && this->m_index.isValid() \
         && value != this->get_##name() \
    ) { \
        this->m_model->setData( \
            this->m_index, \
            QVariant::fromValue(value), \
            role \
        ); \
    } \
}

#include "backend/dataitems/task_roles.inc"

#undef X
