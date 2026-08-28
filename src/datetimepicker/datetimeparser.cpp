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

#include "datetimeparser.h"

#include <QDate>
#include <QLocale>
#include <QPair>
#include <QRegularExpression>
#include <QString>
#include <QTime>
#include <QtTypes>

DateTimeParser::DateTimeParser(QObject* parent) : QObject(parent) {}

void DateTimeParser::set_default_time(const QTime& time)
{
    this->default_time = time.isValid() ? time : QTime(0, 0);
}

QRegularExpressionMatch DateTimeParser::match_shift_pattern(const QString& input)
{
    static const QRegularExpression shift_pattern = QRegularExpression(
        R"(^([+-]?)\s*(\d+)\s*([dwmy]?)$)",
        QRegularExpression::CaseInsensitiveOption
    );
    return shift_pattern.match(input);
}

QDateTime DateTimeParser::parse_shift_pattern_match(const QRegularExpressionMatch& match) const
{
    QDateTime base_date_time = QDateTime(QDate::currentDate(), this->default_time);

    const int sign = (match.captured(1) == "-") ? -1 : 1;
    const int value = sign* match.captured(2).toInt();
    const QString unit = match.captured(3).toLower();

    if (unit == "d" or unit == "")
    {
        base_date_time = base_date_time.addDays(value);
    }
    else if (unit == "w")
    {
        base_date_time = base_date_time.addDays(Q_INT64_C(7)* value);
    }
    else if (unit == "m")
    {
        base_date_time = base_date_time.addMonths(value);
    }
    else if (unit == "y")
    {
        base_date_time = base_date_time.addYears(value);
    }
    else
    {
        return {};
    }
    return base_date_time;
}

QStringList DateTimeParser::year_variants(const QString& format)

{
    QStringList variants;
    variants << format;

    if (format.contains("yyyy"))
    {
        variants << QString(format).replace("yyyy", "yy");
    }
    else if (format.contains("yy"))
    {
        variants << QString(format).replace("yy", "yyyy");
    }

    variants.removeDuplicates();
    return variants;
}

QDateTime DateTimeParser::try_parse_date_time(const QString& input) const
{
    const QString base_format = this->locale.dateTimeFormat(QLocale::ShortFormat);

    for (const QString& format : DateTimeParser::year_variants(base_format))
    {
        QDateTime datetime = this->locale.toDateTime(input, format, DateTimeParser::BASE_YEAR);
        if (datetime.isValid())
        {
            return datetime;
        }
    }

    return {};
}

QDate DateTimeParser::try_parse_date(QString input) const
{
    QString base_format = this->locale.dateFormat(QLocale::ShortFormat);

    for (const QString& format : DateTimeParser::year_variants(base_format))
    {
        QDate date = this->locale.toDate(input, format, DateTimeParser::BASE_YEAR);
        if (date.isValid())
        {
            return date;
        }
    }

    // Try parsing date without year specification:
    base_format.replace(QRegularExpression(R"([^\w]*y{2,4}[^\w]*)"), "");
    base_format = base_format.trimmed();

    input.remove(QRegularExpression(R"(^[^\d]+|[^\d]+$)"));

    QDate date = locale.toDate(input, base_format);
    if (date.isValid())
    {
        date.setDate(QDate::currentDate().year(), date.month(), date.day());
        return date;
    }

    return {};
}

QTime DateTimeParser::try_parse_time(const QString& input) const
{
    if (auto time = this->locale.toTime(input, QLocale::ShortFormat); time.isValid())
    {
        return time;
    }

    static const QStringList fallback_formats =
    {
        "HH:mm",
        "HH:mm:ss"
    };

    for (const auto& format : fallback_formats)
    {
        if (auto time = QTime::fromString(input, format); time.isValid())
        {
            return time;
        }
    }

    return {};
}

QDateTime DateTimeParser::parse_regular_formats(QString& input) const
{
    static const QRegularExpression single_digit_pattern = QRegularExpression(R"(\b(\d)\b)");
    input.replace(single_digit_pattern, "0\\1");

    if (auto datetime = this->try_parse_date_time(input); datetime.isValid())
    {
        return datetime;
    }

    if (auto date = this->try_parse_date(input); date.isValid())
    {
        return { date, this->default_time };
    }

    if (auto time = this->try_parse_time(input); time.isValid())
    {
        return { QDate::currentDate(), time };
    }

    return {};
}

QDateTime DateTimeParser::parse(QString input) const
{
    input = input.simplified();
    if (input.isEmpty())
    {
        return {};
    }

    const auto shift_match = DateTimeParser::match_shift_pattern(input);
    if (shift_match.hasMatch())
    {
        return this->parse_shift_pattern_match(shift_match);
    }

    return this->parse_regular_formats(input);
}
