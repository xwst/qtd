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

#ifndef TESTQTDID_H
#define TESTQTDID_H

#include <functional>

#include <QObject>
#include <QTest>

#include "dataitems/qtdid.h"

class TestQtdId : public QObject
{
    Q_OBJECT
public:
    explicit TestQtdId(QObject *parent = nullptr);

private:
    static void helper_test_conversion(
        const std::function<QVariant(QtdId)>& convert_id
    );

private slots:
    static void initTestCase();

    static void test_creation_from_string();
    static void test_implicit_qvariant_conversion();
    static void test_qvariant_conversion();
    static void test_database_serialization();
};

#endif // TESTQTDID_H
