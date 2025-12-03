/**
 * Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#ifndef SQLRESULTVIEW_H
#define SQLRESULTVIEW_H

#include <iterator>
#include <ranges>
#include <utility>

#include <QSqlQuery>

#include "sqlresultiterator.h"

template <typename T>
class SqlResultView : public std::ranges::view_interface<SqlResultView<T>>
{
    std::ranges::subrange<
        SqlResultIterator<T>,
        typename SqlResultIterator<T>::sentinel_type
    > range;

public:
    SqlResultView() = default;

    explicit constexpr SqlResultView(QSqlQuery&& query)
        : range(SqlResultIterator<T>(std::move(query)), SqlResultIterator<T>::end()) {}

    constexpr SqlResultIterator<T>                begin() const { return range.begin(); }
    constexpr SqlResultIterator<T>::sentinel_type end()   const { return range.end();   }

    static_assert(
        std::sentinel_for<
            typename SqlResultIterator<T>::sentinel_type,
            SqlResultIterator<T>
        >
    );
};


#endif // SQLRESULTVIEW_H
