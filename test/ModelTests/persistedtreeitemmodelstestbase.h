#ifndef PERSISTEDTREEITEMMODELSTESTBASE_H
#define PERSISTEDTREEITEMMODELSTESTBASE_H

#include <QObject>

class PersistedTreeItemModelsTestBase : public QObject
{
    Q_OBJECT

protected:
    QString db_connection_name;

    void clear_table(QString table_name);
    static bool compare_indices(const QModelIndex& index_1, const QModelIndex& index_2);

public:
    explicit PersistedTreeItemModelsTestBase(QObject *parent = nullptr);


protected slots:
    // Test setup/cleanup:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
};

#endif // PERSISTEDTREEITEMMODELSTESTBASE_H
