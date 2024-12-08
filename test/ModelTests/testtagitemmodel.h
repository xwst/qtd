#ifndef TESTTAGITEMMODEL_H
#define TESTTAGITEMMODEL_H

#include <memory>

#include "../../src/app/model/TagItemModel.h"

class TestTagItemModel : public QObject
{
    Q_OBJECT
public:
    explicit TestTagItemModel(QObject *parent = nullptr);

private:
    std::unique_ptr<TagItemModel> model;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void initial_dataset_represented_correctly();
    void remove_rows();
};

#endif // TESTTAGITEMMODEL_H
