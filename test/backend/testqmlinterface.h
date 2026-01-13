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

#include <memory>

#include <QAbstractItemModel>
#include <QObject>
#include <QTemporaryFile>
#include <QTest>

#include "../qmlinterface.h"

class TestQmlInterface : public QObject
{
    Q_OBJECT
private:
    std::unique_ptr<QmlInterface> qml_interface;
    QTemporaryFile temp_db_file;

    void set_up_db();
    void set_up_model_testers(std::initializer_list<const char*> model_names);
    QAbstractItemModel* get_model(const char* model_name) const;

public:
    explicit TestQmlInterface(QObject *parent = nullptr);

private slots:
    // Test setup/cleanup:
    void initTestCase();
    static void cleanupTestCase();

    void test_model_size() const;
    void test_tag_filtering() const;
};
