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

#include <QObject>

class PersistedTreeItemModelsTestBase : public QObject
{
    Q_OBJECT

private:
    QString db_connection_name;

protected:
    static void clear_table(const QString& table_name);
    [[nodiscard]] QString get_db_connection_name() const;

public:
    explicit PersistedTreeItemModelsTestBase(QObject *parent = nullptr);

protected slots:
    // Test setup/cleanup:
    void initTestCase();
    static void cleanupTestCase();
    static void init();
    static void cleanup();
};
