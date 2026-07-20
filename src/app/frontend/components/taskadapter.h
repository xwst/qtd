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

#pragma once

#include <QAbstractItemModel>
#include <QDateTime>
#include <QModelIndex>
#include <QObject>
#include <QQmlEngine>
#include <QTextDocument>
#include <QVariant>

// NOLINTBEGIN(misc-include-cleaner)
#include "dataitems/qtdid.h"
#include "dataitems/qtditemdatarole.h"
#include "dataitems/task.h"
// NOLINTEND(misc-include-cleaner)

class TaskAdapter : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariant index READ get_index WRITE set_index NOTIFY indexChanged)
    Q_PROPERTY(QAbstractItemModel* model READ get_model WRITE set_model NOTIFY modelChanged REQUIRED)

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define X(type, name, role) \
        Q_PROPERTY(type name READ get_##name WRITE set_##name NOTIFY name##Changed)
    #include "dataitems/task_roles.inc"
    #undef X

private:
    QModelIndex m_index;
    QAbstractItemModel* m_model = nullptr;

    [[nodiscard]] bool index_is_new_and_valid(const QModelIndex& idx);
    [[nodiscard]] QVariant data(int role) const;
    void emit_all_changed();

public:
    explicit TaskAdapter(QObject *parent = nullptr) : QObject(parent) {}

    [[nodiscard]] QAbstractItemModel* get_model() const;
    [[nodiscard]] QVariant get_index() const;
    void set_model(QAbstractItemModel* model);
    void set_index(const QVariant& value);

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define X(type, name, role) \
        [[nodiscard]] type get_##name() const; \
        void set_##name(const type& value);
    #include "dataitems/task_roles.inc"
    #undef X

signals:
    void indexChanged();
    void modelChanged();

    void uuidChanged();
    void titleChanged();
    void statusChanged();
    void startChanged();
    void dueChanged();
    void resolveChanged();
    void plainTextChanged();
    void richTextChanged();
    void tagsChanged();

private slots:
    void on_data_changed(
        const QModelIndex& top_left,
        const QModelIndex& bottom_right,
        const QList<int>& roles
    );
};
