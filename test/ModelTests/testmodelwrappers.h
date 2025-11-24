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

/**
 * @file Provide auxiliary derivatives of model classes with
 * private or protected members exposed to facilitate unit testing.
 *
 */

#ifndef TESTMODELWRAPPERS_H
#define TESTMODELWRAPPERS_H

#include "models/treeitemmodel.h"

class TreeItemModelTestWrapper : public TreeItemModel {
public:
    using TreeItemModel::create_tree_node;
    using TreeItemModel::clone_tree_node;
};

#endif // TESTMODELWRAPPERS_H
