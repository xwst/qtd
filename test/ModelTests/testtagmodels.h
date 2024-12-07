#ifndef TESTTAGMODELS_H
#define TESTTAGMODELS_H

#include <memory>
#include <QTest>

#include "../../src/app/model/TagItemModel.h"

class TestTagModels : public QObject
{
    Q_OBJECT
public:
    explicit TestTagModels(QObject *parent = nullptr);

private:
    std::unique_ptr<TagItemModel> model;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void initial_dataset_represented_correctly();
};

#endif // TESTTAGMODELS_H
