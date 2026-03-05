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

#include "datetimevalidator.h"

#include <QLocale>
#include <QRegularExpression>
#include <QValidator>

namespace {


} // anonymous namespace

DateTimeValidator::DateTimeValidator(const QLocale& locale, QObject* parent)
    : QValidator(parent), locale(locale) {}

DateTimeValidator::State DateTimeValidator::validate(QString& input, int& /* pos */) const {
    const QDateTime date_time = locale.toDateTime(input, QLocale::ShortFormat);
    if (date_time.isValid()) {
        input = locale.toString(date_time, QLocale::ShortFormat);
        return QValidator::Acceptable;
    }
    return QValidator::Intermediate;
}

bool DateTimeValidator::parse_date_time_with_two_digit_year(QString &input) const {
    QString short_format = this->locale.dateTimeFormat(QLocale::ShortFormat).replace("yyyy", "yy");
    const QDateTime date_time = locale.toDateTime(input, short_format, 2000);
    if (date_time.isValid()) {
        input = locale.toString(date_time, QLocale::ShortFormat);
        return true;
    }
    return false;
}

void DateTimeValidator::parse_shift_pattern_match(
    const QRegularExpressionMatch& match,
    QString& input
) const {
    QDateTime fixed_date_time = this->base_date_time;

    const int sign = (match.captured(1) == "-") ? -1 : 1;
    const int value = sign * match.captured(2).toInt();
    const QString unit = match.captured(3).toLower();

    if (unit == "d" or unit == "") {
        fixed_date_time = fixed_date_time.addDays(value);
    } else if (unit == "w") {
        fixed_date_time = fixed_date_time.addDays(Q_INT64_C(7) * value);
    } else if (unit == "m") {
        fixed_date_time = fixed_date_time.addMonths(value);
    } else if (unit == "y") {
        fixed_date_time = fixed_date_time.addYears(value);
    } else {
        return;
    }
    input = locale.toString(fixed_date_time, QLocale::ShortFormat);
}

void DateTimeValidator::fixup(QString& input) const {
    static const QRegularExpression shift_pattern = QRegularExpression(
        R"(^\s*([+-]?)\s*(\d+)\s*([dwmy]?)\s*$)",
        QRegularExpression::CaseInsensitiveOption
    );
    const QRegularExpressionMatch shift_match = shift_pattern.match(input);
    if (shift_match.hasMatch()) {
        this->parse_shift_pattern_match(shift_match, input);
    } else {
        static const QRegularExpression single_digit_pattern = QRegularExpression(R"(\b(\d)\b)");
        input.replace(single_digit_pattern, "0\\1");
        int pos = 0;
        if (
            this->validate(input, pos) != QValidator::Acceptable
            && !this->parse_date_time_with_two_digit_year(input)
        ) {
            input = locale.toString(this->current_date_time, QLocale::ShortFormat);
        }
    }
}
