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

#ifndef TESTFILTEREDTAGITEMMODEL_H
#define TESTFILTEREDTAGITEMMODEL_H

#include <memory>

#include <QObject>

#include "../../src/app/model/filteredtagitemmodel.h"
#include "../../src/app/model/tagitemmodel.h"

class TestFilteredTagItemModel : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<TagItemModel> base_model;
    std::unique_ptr<FilteredTagItemModel> model;

    void whitelist_single_parent_tag_and_validate() const;

public:
    explicit TestFilteredTagItemModel(QObject *parent = nullptr);

private slots:
    // Test setup/cleanup:
    void initTestCase();

    void test_empty_whitelist_yields_empty_model() const;
    void test_whitelisting_parent() const;
    void test_whitelisting_children() const;
    void test_removing_tags_from_whitelist() const;

};

#endif // TESTFILTEREDTAGITEMMODEL_H
