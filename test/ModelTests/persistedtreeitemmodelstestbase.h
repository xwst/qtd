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

#ifndef PERSISTEDTREEITEMMODELSTESTBASE_H
#define PERSISTEDTREEITEMMODELSTESTBASE_H

#include <QObject>

class PersistedTreeItemModelsTestBase : public QObject
{
    Q_OBJECT

protected:
    QString db_connection_name;

    void clear_table(QString table_name);

public:
    explicit PersistedTreeItemModelsTestBase(QObject *parent = nullptr);


protected slots:
    // Test setup/cleanup:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
};

#endif // PERSISTEDTREEITEMMODELSTESTBASE_H
