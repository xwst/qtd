#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <QAbstractItemModel>
#include <QAbstractItemModelTester>
#include <QString>
#include <QStringList>

class TestHelpers
{
private:
    static std::vector<QModelIndex> get_sorted_children(
        const QAbstractItemModel& model,
        const QModelIndex& parent,
        const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator
    );
public:
    static void setup_database();
    static void assert_table_exists(const QString& table_name);
    static void populate_database();
    static void assert_model_equality(
        const QAbstractItemModel& model_under_test,
        const QAbstractItemModel& model_expectation,
        const QSet<int>& roles_to_check,
        const std::function<bool(const QModelIndex&, const QModelIndex&)>& item_sort_comparator,
        const QModelIndex& index_of_model_under_test = QModelIndex(),
        const QModelIndex& index_of_model_expectation = QModelIndex()
    );

    template <typename T, typename... Types>
    static void setup_item_model(
        std::unique_ptr<T>& model,
        Types... constructor_parameters
    ) {
        model = std::make_unique<T>(constructor_parameters...);
        new QAbstractItemModelTester(model.get(), model.get());
        // ModelTester will be deleted by the models destructor.
    }

    template <typename T, typename... Types>
    static void setup_proxy_item_model(
        std::unique_ptr<T>& model,
        QAbstractItemModel* source_model,
        Types... constructor_parameters
    ) {
        model = std::make_unique<T>(constructor_parameters...);
        model->setSourceModel(source_model);
        new QAbstractItemModelTester(model.get(), source_model);
        // ModelTester will be deleted by the models destructor.
    }

    static QStringList get_display_roles(
        const QAbstractItemModel& model,
        const QModelIndex& parent = QModelIndex()
    );
};

#endif // TESTHELPERS_H
