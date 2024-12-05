#ifndef TESTTAGMODELS_H
#define TESTTAGMODELS_H

#include <QTest>

class TestTagModels : public QObject
{
    Q_OBJECT
public:
    explicit TestTagModels(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void tag_table_exists();
};

#endif // TESTTAGMODELS_H
