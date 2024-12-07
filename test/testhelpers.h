#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <QString>

class TestHelpers
{
public:
    static void setup_database();
    static void assert_table_exists(const QString& table_name);
    static void populate_database();
};

#endif // TESTHELPERS_H
