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

#include <QDateTime>
#include <QLocale>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QValidator>

class DateTimeValidator : public QValidator {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QLocale locale MEMBER locale)
    Q_PROPERTY(QDateTime base_date_time MEMBER base_date_time REQUIRED)
    Q_PROPERTY(QDateTime current_date_time MEMBER current_date_time REQUIRED)

private:
    QLocale locale;
    QDateTime base_date_time;
    QDateTime current_date_time;

    bool parse_date_time_with_two_digit_year(QString& input) const;
    void parse_shift_pattern_match(const QRegularExpressionMatch& match, QString& input) const;

public:
    explicit DateTimeValidator(
        const QLocale& locale = QLocale(),
        QObject *parent = nullptr
    );

    State validate(QString& input, int& pos) const override;
    void fixup(QString& input) const override;
};
