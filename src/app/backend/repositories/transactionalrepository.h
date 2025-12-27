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

#pragma once

#include <initializer_list>

#include <QSqlQuery>
#include <QString>

class TransactionalRepository
{
private:
    QString connection_name;
    bool rollback_requested;

protected:
    explicit TransactionalRepository(const QString& database_connection_name);

    [[nodiscard]] const QString& get_connection_name() const;
    // NOLINTNEXTLINE (modernize-use-nodiscard)
    bool alter_database(
        const QString& sql_file_name,
        std::initializer_list<QVariant> bind_values,
        bool batch = false,
        const QString& replace_pattern = "",
        const QString& replace_string = ""
    ) const;

public:
    TransactionalRepository(const TransactionalRepository&)             = delete;
    TransactionalRepository(TransactionalRepository&&)                  = delete;
    TransactionalRepository& operator=(TransactionalRepository& other)  = delete;
    TransactionalRepository& operator=(TransactionalRepository&& other) = delete;
    ~TransactionalRepository();

    void roll_back();
    bool roll_back_on_failure(bool execution_result);
};
