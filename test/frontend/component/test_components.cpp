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

#include <memory>

// Includes are required but not all used directly, as the macros are defined
// in referenced header files
// NOLINTNEXTLINE(misc-include-cleaner)
#include <QQmlContext>
#include <QQmlEngine>
#include <QStringListModel>
// NOLINTNEXTLINE(misc-include-cleaner)
#include <QtQuickTest>

class SetupComponentTests : public QObject
{
    Q_OBJECT

    std::unique_ptr<QStringListModel> dummy_model;

public:
    SetupComponentTests() = default;

public slots:
    void qmlEngineAvailable(QQmlEngine *engine) {
        this->dummy_model = std::make_unique<QStringListModel>();
        QStringList tmp;
        tmp << "Name 1" << "Name 2" << "Name 3";
        this->dummy_model->setStringList(tmp);
        engine->rootContext()->setContextProperty("dummyIndex", this->dummy_model->index(0));
    }
};



QUICK_TEST_MAIN_WITH_SETUP(test_components, SetupComponentTests)

#include "test_components.moc"
