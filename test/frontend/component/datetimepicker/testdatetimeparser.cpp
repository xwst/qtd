/**
 * Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include "testdatetimeparser.h"

#include <memory>

#include <QDate>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QtTest>

#include "datetimeparser.h"

// -------------------------------------------------
// helper utilities
// -------------------------------------------------

std::unique_ptr<DateTimeParser> TestDateTimeParser::make_parser(const QLocale& locale) const {
    QLocale::setDefault(locale);
    auto parser = std::make_unique<DateTimeParser>();
    parser->setProperty("default_time", this->defaultTime);
    return parser;
}

QList<QLocale> TestDateTimeParser::locales() {
    return {
        QLocale::C,
        QLocale(QLocale::English, QLocale::UnitedStates),
        QLocale(QLocale::German, QLocale::Germany)
    };
}

// -------------------------------------------------
// SHIFT TESTS
// -------------------------------------------------

void TestDateTimeParser::shift_units() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = this->make_parser(loc);

        QCOMPARE(parser->parse("+1d").date(), today.addDays(1));
        QCOMPARE(parser->parse("-1d").date(), today.addDays(-1));

        QCOMPARE(parser->parse("+2w").date(), today.addDays(14));
        QCOMPARE(parser->parse("-3w").date(), today.addDays(-21));

        QCOMPARE(parser->parse("+1m").date(), today.addMonths(1));
        QCOMPARE(parser->parse("-2m").date(), today.addMonths(-2));

        QCOMPARE(parser->parse("+1y").date(), today.addYears(1));
        QCOMPARE(parser->parse("-2y").date(), today.addYears(-2));
    }
}

void TestDateTimeParser::shift_implicit_days() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        QCOMPARE(parser->parse("3").date(), today.addDays(3));
        QCOMPARE(parser->parse("+5").date(), today.addDays(5));
        QCOMPARE(parser->parse("-4").date(), today.addDays(-4));
    }
}

void TestDateTimeParser::shift_whitespace() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        QCOMPARE(parser->parse("+ 3 d").date(), today.addDays(3));
        QCOMPARE(parser->parse("  - 2 w ").date(), today.addDays(-14));
    }
}

void TestDateTimeParser::shift_large_values() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        QCOMPARE(parser->parse("+365d").date(), today.addDays(365));
        QCOMPARE(parser->parse("-365d").date(), today.addDays(-365));
    }
}

// -------------------------------------------------
// LOCALE PARSING
// -------------------------------------------------

void TestDateTimeParser::locale_dates() {
    const QDate sample(2024, 3, 14);

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        QString text = loc.toString(sample, QLocale::ShortFormat);
        auto result = parser->parse(text);

        QVERIFY(result.isValid());
        QCOMPARE(result.date(), sample);
        QCOMPARE(result.time(), this->defaultTime);
    }
}

void TestDateTimeParser::locale_datetimes() {
    const QDateTime sample(QDate(2024,6,7), QTime(13,45));

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        auto result = parser->parse(loc.toString(sample, QLocale::ShortFormat));

        QVERIFY(result.isValid());
        QCOMPARE(result.date(), sample.date());
        QCOMPARE(result.time(), sample.time());
    }
}

// -------------------------------------------------
// TIME
// -------------------------------------------------

void TestDateTimeParser::time_only() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);
        auto result = parser->parse("13:45");

        QVERIFY(result.isValid());
        QCOMPARE(result.date(), today);
        QCOMPARE(result.time(), QTime(13, 45));
    }
}

// -------------------------------------------------
// DATE WITHOUT YEAR
// -------------------------------------------------

void TestDateTimeParser::date_without_year() {
    const int current_year = QDate::currentDate().year();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto format = loc.dateFormat(QLocale::ShortFormat)
                               .replace(QRegularExpression("[^\\d]?y{1-4}[^\\d]?"), "");
        const auto input = loc.toString(QDate(current_year, 3, 14), format);
        const auto parser = make_parser(loc);
        const auto result = parser->parse(input);

        QVERIFY(result.isValid());
        QCOMPARE(result.date().year(), current_year);
    }
}

// -------------------------------------------------
// YEAR VARIANTS
// -------------------------------------------------

void TestDateTimeParser::year_variants() {
    const QDate sample(2024, 5, 6);

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        const QString format = loc.dateFormat(QLocale::ShortFormat);

        if (!format.contains('y')) {
            continue;
        }

        QString format_yyyy = format;
        QString format_yy   = format;

        format_yyyy.replace(QRegularExpression("y{1,4}"), "yyyy");
        format_yy.replace(QRegularExpression("y{1,4}"), "yy");

        QString text_yyyy = sample.toString(format_yyyy);
        QString text_yy   = sample.toString(format_yy);

        auto result_yyyy = parser->parse(text_yyyy);
        auto result_yy = parser->parse(text_yy);

        QVERIFY(result_yyyy.isValid());
        QVERIFY(result_yy.isValid());

        QCOMPARE(result_yyyy.date(), sample);
        QCOMPARE(result_yy.date().year() % 100, sample.year() % 100);
    }
}

// -------------------------------------------------
// ROLLOVER
// -------------------------------------------------

void TestDateTimeParser::month_rollover() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);
        QCOMPARE(parser->parse("+1m").date(), today.addMonths(1));
    }
}

void TestDateTimeParser::year_rollover() {
    const QDate today = QDate::currentDate();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);
        QCOMPARE(parser->parse("+1y").date(), today.addYears(1));
    }
}

// -------------------------------------------------
// LEAP YEAR
// -------------------------------------------------

void TestDateTimeParser::leap_year_dates() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        const QDate leap(2024, 2, 29);
        const QString text = loc.toString(leap, QLocale::ShortFormat);

        const auto result = parser->parse(text);

        QVERIFY(result.isValid());
        QCOMPARE(result.date(), leap);
    }
}

// -------------------------------------------------
// WHITESPACE
// -------------------------------------------------

void TestDateTimeParser::whitespace_handling() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        const auto result_padded = parser->parse("   +3d ");
        const auto result = parser->parse("+3d");

        QCOMPARE(result_padded.date(), result.date());
    }
}

// -------------------------------------------------
// INVALID INPUTS
// -------------------------------------------------

void TestDateTimeParser::invalid_inputs() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        QVERIFY(!parser->parse("").isValid());
        QVERIFY(!parser->parse(" ").isValid());
        QVERIFY(!parser->parse("notadate").isValid());
        QVERIFY(!parser->parse("++--").isValid());
    }
}

// -------------------------------------------------
// NORMALIZATION TESTS
// -------------------------------------------------

void TestDateTimeParser::normalization_time_single_digits() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        const auto result = parser->parse("3:4");
        QVERIFY(result.isValid());
        QCOMPARE(result.time(), QTime(3, 4));
    }
}

void TestDateTimeParser::normalization_time_mixed_digits() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        auto result = parser->parse("3:45");
        QVERIFY(result.isValid());
        QCOMPARE(result.time(), QTime(3, 45));
    }
}

void TestDateTimeParser::normalization_time_leading_zero_preserved() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        const auto result = parser->parse("03:04");
        QVERIFY(result.isValid());
        QCOMPARE(result.time(), QTime(3, 4));
    }
}

void TestDateTimeParser::normalization_date_single_digits() {
    const QDate sample(2024, 3, 7);

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        const QString format_d  = loc.dateFormat(QLocale::ShortFormat).replace("dd", "d");
        const QString format_dd = QString(format_d).replace("d", "dd");
        const QString text_d    = loc.toString(sample, format_d);
        const QString text_dd   = loc.toString(sample, format_dd);
        const auto result_d     = parser->parse(text_d);
        const auto result_dd    = parser->parse(text_dd);

        QVERIFY(result_d.isValid());
        QCOMPARE(result_d.date(), sample);
        QVERIFY(result_dd.isValid());
        QCOMPARE(result_dd.date(), sample);
    }
}

void TestDateTimeParser::normalization_datetime_single_digits() {
    const QDateTime sample(QDate(2024, 3, 7), QTime(4, 5));

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        auto default_format = loc.dateTimeFormat(QLocale::ShortFormat);
        const QString format_single = default_format.replace("dd", "d").replace("HH", "H");
        const QString format_double = QString(format_single).replace("d", "dd").replace("H", "HH");
        const QString text_single   = loc.toString(sample, format_single);
        const QString text_double   = loc.toString(sample, format_double);
        auto result_single    = parser->parse(text_single);
        auto result_double    = parser->parse(text_double);

        QVERIFY(result_single.isValid());
        QCOMPARE(result_single, sample);
        QVERIFY(result_double.isValid());
        QCOMPARE(result_double, sample);
    }
}

void TestDateTimeParser::normalization_does_not_change_double_digits() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);
        const auto result = parser->parse("12:34");

        QVERIFY(result.isValid());
        QCOMPARE(result.time(), QTime(12, 34));
    }
}

void TestDateTimeParser::normalization_date_without_year_single_digits() {
    const int currentYear = QDate::currentDate().year();

    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);
        const auto r = parser->parse("3/7");

        if (r.isValid()) {
            QCOMPARE(r.date().month(), 3);
            QCOMPARE(r.date().day(), 7);
            QCOMPARE(r.date().year(), currentYear);
        }
    }
}

void TestDateTimeParser::normalization_boundary_values() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        for (const auto& input : QStringList({ "0:0", "9:9" })) {
            const auto result = parser->parse(input);
            QVERIFY(result.isValid());
            const int digit = input.at(0).digitValue();
            QCOMPARE(result.time(), QTime(digit, digit));
        }
    }
}

void TestDateTimeParser::normalization_invalid_results() {
    for (const auto& loc : TestDateTimeParser::locales()) {
        const auto parser = make_parser(loc);

        QVERIFY(!parser->parse("9:99").isValid());
        QVERIFY(!parser->parse("99:9").isValid());
    }
}

QTEST_MAIN(TestDateTimeParser)