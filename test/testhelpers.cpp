#include "testhelpers.h"

#include <vector>
#include <QTest>

#include "../src/app/util.h"

void TestHelpers::setup_database() {
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    Util::create_tables_if_not_exist(db.connectionName());
}

void TestHelpers::assert_table_exists(const QString& table_name) {
    QSqlQuery query;
    bool result = query.exec("SELECT COUNT(*) FROM " + table_name);
    QString error_msg = QString("The '%1'-table was not created!").arg(table_name);
    QVERIFY2(result, qPrintable(error_msg));
}

void TestHelpers::populate_database() {
    QFile f(":/resources/sql/generic/populate_database.sql");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&f);
    QString all_queries_str = in.readAll();

    QSqlQuery query;
    for (QString& query_str : Util::split_queries(all_queries_str))
        QVERIFY2(query.exec(query_str), "Error while populating test database");
}

std::vector<QModelIndex> TestHelpers::get_sorted_children(
    const QAbstractItemModel& model,
    const QModelIndex& parent,
    const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator
) {
    std::vector<QModelIndex> result;
    int number_of_children = model.rowCount(parent);
    int column = parent.isValid() ? parent.column() : 0;

    result.reserve(number_of_children);
    for (int row=0; row<number_of_children; row++)
        result.push_back(model.index(row, column, parent));
    std::sort(result.begin(), result.end(), item_sort_comparator);

    return result;
}

void TestHelpers::assert_model_equality(
    const QAbstractItemModel& model_under_test,
    const QAbstractItemModel& model_expectation,
    const QSet<Qt::ItemDataRole>& roles_to_check,
    const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator,
    const QModelIndex& index_of_model_under_test,
    const QModelIndex& index_of_model_expectation
) {
    // Sanity checks:
    QCOMPARE_NE(&model_under_test, &model_expectation);
    QVERIFY(model_expectation.checkIndex(index_of_model_expectation));
    QVERIFY(model_under_test.checkIndex(index_of_model_under_test));

    // Index data equality:
    QCOMPARE(
        index_of_model_under_test.isValid(),
        index_of_model_expectation.isValid()
    );
    for (auto item_data_role : roles_to_check)
        QCOMPARE(
            index_of_model_under_test.data(item_data_role),
            index_of_model_expectation.data(item_data_role)
        );

    // Dimension equality:
    int row_count_test_model = model_under_test.rowCount(index_of_model_under_test);
    int column_count_test_model = model_under_test.columnCount(index_of_model_under_test);
    QCOMPARE(
        row_count_test_model,
        model_expectation.rowCount(index_of_model_expectation)
    );
    QCOMPARE(
        column_count_test_model,
        model_expectation.columnCount(index_of_model_expectation)
    );

    // Sort children by uuid:
    auto children_under_test = TestHelpers::get_sorted_children(
        model_under_test, index_of_model_under_test, item_sort_comparator
    );
    auto children_expectation = TestHelpers::get_sorted_children(
        model_expectation, index_of_model_expectation, item_sort_comparator
    );

    // Children equality (recursion):
    for (int i=0; i<children_under_test.size(); i++)
        TestHelpers::assert_model_equality(
            model_under_test,
            model_expectation,
            roles_to_check,
            item_sort_comparator,
            children_under_test[i],
            children_expectation[i]
        );
}

QStringList TestHelpers::get_display_roles(
    const QAbstractItemModel& model,
    const QModelIndex& parent
) {
    QStringList result;
    if (parent.isValid()) result.append(parent.data().toString());

    auto column = parent.isValid() ? parent.column() : 0;
    for (int i=0; i<model.rowCount(parent); i++)
        result.append(get_display_roles(model, model.index(i, column, parent)));
    return result;
}
