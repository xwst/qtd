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

.pragma library

function findChild(selector, root_item) {
    for (var index in root_item.children) {
        if (selector(root_item.children[index]))
            return root_item.children[index]

        var result = findChild(selector, root_item.children[index])
        if (result != undefined)
            return result
    }
}

function findChildByType(type, root_item) {
    return findChild((child) => child.toString().includes(type), root_item)
}

function findChildByText(text, root_item) {
    return findChild((child) => child.text == text, root_item)
}
