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

#include "settings.h"

#include <QDir>
#include <QSqlDatabase>
#include <QStandardPaths>

#include "util.h"

void Settings::open_database(QString& connection_name) {
    QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    dir.mkpath(".");

    auto db = QSqlDatabase::addDatabase("QSQLITE", "local");
    db.setDatabaseName(dir.absoluteFilePath(this->local_database_name));
    Util::create_tables_if_not_exist(connection_name);
}

void Settings::set_up_models() {
    QString connection_name = "local";
    this->open_database(connection_name);
    this->m_tags_model = new TagItemModel(connection_name, this);
    this->m_flat_tags_model = new FlatteningProxyModel(this);
    this->m_flat_tags_model->setSourceModel(this->m_tags_model);
    this->m_task_model = new TaskItemModel(connection_name, this);
}
