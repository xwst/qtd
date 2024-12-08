#ifndef TESTFLATTENINGPROXYMODEL_H
#define TESTFLATTENINGPROXYMODEL_H

#include <QObject>

#include "../../src/app/model/flatteningproxymodel.h"
#include "../../src/app/model/TagItemModel.h"

class TestFlatteningProxyModel : public QObject
{
    Q_OBJECT
public:
    explicit TestFlatteningProxyModel(QObject *parent = nullptr);

private:
    std::unique_ptr<TagItemModel> tag_model;
    std::unique_ptr<FlatteningProxyModel> model;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void initial_dataset_represented_correctly();
    void remove_rows();
};

#endif // TESTFLATTENINGPROXYMODEL_H
