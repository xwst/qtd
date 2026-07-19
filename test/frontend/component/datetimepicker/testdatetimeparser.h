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

#pragma once

#include <memory>

#include <QLocale>
#include <QObject>
#include <QTime>

#include "datetimeparser.h"

class TestDateTimeParser : public QObject
{
    Q_OBJECT

private:
    const QTime defaultTime = QTime(8, 15);

    static QList<QLocale> locales();
    [[nodiscard]] std::unique_ptr<DateTimeParser> make_parser(const QLocale& locale) const;

private slots:
    // ---- shift tests ----
    void shift_units();
    void shift_implicit_days();
    void shift_whitespace();
    void shift_large_values();

    // ---- locale parsing ----
    void locale_dates();
    void locale_datetimes();

    // ---- time parsing ----
    void time_only();

    // ---- date behavior ----
    void date_without_year();
    void year_variants();

    // ---- rollover ----
    void month_rollover();
    void year_rollover();

    // ---- leap year ----
    void leap_year_dates();

    // ---- whitespace ----
    void whitespace_handling();

    // ---- invalid ----
    void invalid_inputs();

    // ---- normalization tests ----
    void normalization_time_single_digits();
    void normalization_time_mixed_digits();
    void normalization_time_leading_zero_preserved();
    void normalization_date_single_digits();
    void normalization_datetime_single_digits();
    void normalization_does_not_change_double_digits();
    void normalization_date_without_year_single_digits();
    void normalization_boundary_values();
    void normalization_invalid_results();
};