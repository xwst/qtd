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

#ifndef SQLRESULTITERATOR_H
#define SQLRESULTITERATOR_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QVariantList>

template <typename T>
class SqlResultIterator {

    static_assert(
        std::is_constructible_v<T, const QVariantList&>,
        "Result type must be constructible using a QVariantList!"
    );

public:
    using iterator_category = std::input_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
    using sentinel_type     = std::default_sentinel_t;

    explicit SqlResultIterator(QSqlQuery&& query) noexcept
        : query(std::make_shared<QSqlQuery>(std::move(query))),
          column_count(this->query->record().count())
    {
        this->query->next();
        build_current();
    }
    static sentinel_type end() noexcept {
        return std::default_sentinel;
    }

    reference operator*()  const noexcept { return *this->current; }
    pointer   operator->() const noexcept { return this->current.get(); }
    [[nodiscard]] QVariant  get_raw(int column) const noexcept {
        return this->query->value(column);
    }

    SqlResultIterator<T>& operator++ () noexcept {
        this->query->next();
        this->build_current();
        return *this;
    }

    SqlResultIterator<T> operator++ (int) noexcept {
        auto result = *this;
        ++(*this);
        return result;
    }

    bool operator==(const std::default_sentinel_t& /*sentinel*/) const noexcept {
        return !this->query->isValid();
    }

    bool operator!=(const std::default_sentinel_t& sentinel) const noexcept {
        return !(*this == sentinel);
    }

    friend bool operator==(
        const std::default_sentinel_t& sentinel,
        const SqlResultIterator<T>& iterator
    ) {
        return iterator == sentinel;
    }

private:
    std::shared_ptr<QSqlQuery> query;
    std::shared_ptr<value_type> current;
    int column_count;

    void build_current() {
        if (this->query->isValid()) {
            QVariantList args(this->column_count);
            for (int column_index=0; column_index<args.size(); ++column_index) {
                args[column_index] = this->query->value(column_index);
            }
            this->current = std::make_shared<value_type>(args);
        } else {
            this->current.reset();
        }
    }
};

#endif // SQLRESULTITERATOR_H
