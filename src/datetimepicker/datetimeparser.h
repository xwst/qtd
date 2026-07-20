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
#include <QObject>
#include <QQmlEngine>
#include <QRegularExpressionMatch>
#include <QString>
#include <QTime>

class DateTimeParser : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QTime default_time MEMBER default_time WRITE set_default_time REQUIRED)

private:
    QLocale locale;
    QTime default_time;
    static constexpr int BASE_YEAR = 2000;

    void set_default_time(const QTime& time);

    [[nodiscard]] static QRegularExpressionMatch match_shift_pattern(const QString& input);

    [[nodiscard]] QDateTime parse_shift_pattern_match(const QRegularExpressionMatch& match) const;
    [[nodiscard]] static QStringList year_variants(const QString& format);
    [[nodiscard]] QDateTime try_parse_date_time(const QString& input) const;
    [[nodiscard]] QDate try_parse_date(QString input) const;
    [[nodiscard]] QTime try_parse_time(const QString& input) const;
    [[nodiscard]] QDateTime parse_regular_formats(QString& input) const;

public:
    explicit DateTimeParser(QObject* parent = nullptr);
    Q_INVOKABLE [[nodiscard]] QDateTime parse(QString input) const;
};
