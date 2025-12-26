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

#ifndef TASKREPOSITORY_H
#define TASKREPOSITORY_H

#include "transactionalrepository.h"

#include <QHash>
#include <QList>
#include <QMultiHash>
#include <QSet>

#include "dataitems/qtdid.h"
#include "dataitems/task.h"
#include "sqlresultview.h"

class TaskRepository : public TransactionalRepository
{
private:
    using TransactionalRepository::TransactionalRepository;

    // NOLINTNEXTLINE (modernize-use-nodiscard)
    bool remove_isolated(const QList<QVariant>& task_ids) const;

public:
    static TaskRepository create(const QString &database_connection_name);

    [[nodiscard]] SqlResultView<Task> get_all_tasks() const;
    [[nodiscard]] QHash<TaskId, QSet<TagId>> get_all_tag_assignments() const;
    [[nodiscard]] QMultiHash<TaskId, TaskId> get_all_dependencies() const;

    // NOLINTBEGIN (modernize-use-nodiscard)
    bool save(const Task& task) const;
    bool update_column(const TaskId& task, const QString& column_name, const QVariant& new_value) const;

    bool add_prerequisites   (const TaskId& dependent,    const QList<QVariant>& prerequisites) const;
    bool add_dependents      (const TaskId& prerequisite, const QList<QVariant>& dependents   ) const;
    bool remove_prerequisites(const TaskId& dependent,    const QList<QVariant>& prerequisites) const;
    bool remove_dependents   (const TaskId& prerequisite, const QList<QVariant>& dependents   ) const;

    bool add_tag   (const TaskId& task, const TagId& tag) const;
    bool remove_tag(const TaskId& task, const TagId& tag) const;
    // NOLINTEND (modernize-use-nodiscard)
};

#endif // TASKREPOSITORY_H
