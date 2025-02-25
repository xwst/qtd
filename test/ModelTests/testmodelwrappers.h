/**
 * @file Provide auxiliary derivatives of model classes with
 * private or protected members exposed to facilitate unit testing.
 *
 */

#ifndef TESTMODELWRAPPERS_H
#define TESTMODELWRAPPERS_H

#include "../../src/app/model/treeitemmodel.h"

class TreeItemModelTestWrapper : public TreeItemModel {
public:
    using TreeItemModel::create_tree_node;
    using TreeItemModel::clone_tree_node;
};

#endif // TESTMODELWRAPPERS_H
