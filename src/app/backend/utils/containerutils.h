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

#ifndef CONTAINERUTILS_H
#define CONTAINERUTILS_H

#include <ranges>
#include <type_traits>

#include <QList>
#include <QSet>

class ContainerUtils
{
public:
    ContainerUtils() = delete;

    /**
     * @brief Helper class to apply a transformation to a container type
     */
    template <template<typename> class Container, typename T, typename Func>
    static Container<typename std::invoke_result_t<Func, const T&>> transform(
        const Container<T>& container,
        Func transformation
    ) {
        using ResultType = typename std::invoke_result_t<Func, const T&>;
        auto view = container | std::views::transform(transformation);

        Container<ResultType> result;
        result.reserve(container.size());
        for (const auto& element : view) {
            if constexpr (std::is_same_v<Container<T>, QList<T>>) {
                result.append(element);
            } else if constexpr (std::is_same_v<Container<T>, QSet<T>>) {
                result.insert(element);
            } else {
                static_assert("Unsupported container type");
            }
        }
        return result;
    }
};

#endif // CONTAINERUTILS_H
