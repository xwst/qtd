/**
 * Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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
#include <utility>

#include <QDateTime>
#include <QModelIndex>
// NOLINTNEXTLINE(misc-include-cleaner)
#include <QQmlContext>
#include <QQmlEngine>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QString>
// NOLINTNEXTLINE(misc-include-cleaner)
#include <QtQuickTest>

#include "../../backend/testmodelwrappers.h"
#include "../frontend/components/taskadapter.h"
#include "../qmlinterface.h"
#include "dataitems/task.h"
#include "models/flatteningproxymodel.h"

class TagEditorTestModel : public QStandardItemModel
{
    Q_OBJECT

public:
    using QStandardItemModel::QStandardItemModel;

    Q_INVOKABLE static bool change_parent(const QModelIndex& /* index */, const QString& /* new_parent */) {
        return true;
    }
};

class SetupComponentTests : public QObject
{
    Q_OBJECT

    std::unique_ptr<TagEditorTestModel> dummy_source_model;
    std::unique_ptr<QSortFilterProxyModel> dummy_proxy_model;
    std::unique_ptr<TreeItemModelTestWrapper> dummy_task_model;

public:
    SetupComponentTests() = default;

public slots:
    void qmlEngineAvailable(QQmlEngine *engine) {
        this->setup_test_models();
        engine->rootContext()->setContextProperty("dummyIndex", this->dummy_proxy_model->index(0, 0));
        engine->rootContext()->setContextProperty("dummyTaskModel", this->dummy_task_model.get());

        // NOLINTBEGIN(cppcoreguidelines-owning-memory)
        auto* adapter = new TaskAdapter(engine);
        adapter->set_model(this->dummy_task_model.get());
        engine->rootContext()->setContextProperty("adapter", adapter);
        // NOLINTEND(cppcoreguidelines-owning-memory)

        this->setup_qml_interface(engine);
    }

private:
    void setup_test_models() {
        this->setup_test_stringlist_model();
        this->setup_test_task_model();
    }

    void setup_qml_interface(QQmlEngine *engine) {
        auto* qml_interface = engine->singletonInstance<QmlInterface*>("src.app", "QmlInterface");
        // NOLINTBEGIN(cppcoreguidelines-owning-memory)
        auto* flat = new FlatteningProxyModel(qml_interface);
        // NOLINTEND(cppcoreguidelines-owning-memory)
        flat->setSourceModel(this->dummy_source_model.get());
        qml_interface->setProperty("flat_tags", QVariant::fromValue(flat));
    }

    void setup_test_stringlist_model() {
        this->dummy_source_model = std::make_unique<TagEditorTestModel>();
        auto* item1 = new QStandardItem("Name 1");
        item1->setData(QColor("lightblue"), Qt::DecorationRole);
        auto* item2 = new QStandardItem("Name 2");
        item2->setData(QColor("lightgreen"), Qt::DecorationRole);
        auto* item3 = new QStandardItem("Name 3");
        item3->setData(QColor("lightyellow"), Qt::DecorationRole);
        this->dummy_source_model->appendRow(item1);
        this->dummy_source_model->appendRow(item2);
        this->dummy_source_model->appendRow(item3);

        this->dummy_proxy_model = std::make_unique<QSortFilterProxyModel>();
        this->dummy_proxy_model->setSourceModel(this->dummy_source_model.get());
    }

    void setup_test_task_model() {
        this->dummy_task_model = std::make_unique<TreeItemModelTestWrapper>();

        auto start_1 = QDateTime::fromString("2025-01-01T12:00:00", Qt::ISODate);
        auto start_2 = QDateTime::fromString("2025-10-10T10:00:00", Qt::ISODate);

        auto test_task_1 = std::make_unique<Task>("Task 1", Task::Status::Open,   start_1);
        auto test_task_2 = std::make_unique<Task>("Task 2", Task::Status::Closed, start_2);

        this->dummy_task_model->create_tree_node(std::move(test_task_1));
        this->dummy_task_model->create_tree_node(std::move(test_task_2));
    }
};



QUICK_TEST_MAIN_WITH_SETUP(test_components, SetupComponentTests)

#include "test_components.moc"
