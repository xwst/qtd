/**
 * Copyright 2024, 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

function find_child_helper(root, name) {
    if (!root) {
        return null
    }

    let queue = [root]

    while (queue.length > 0) {
        const item = queue.shift()

        if (item.objectName === name) {
            return item
        }

        if (item.children && item.children.length > 0) {
            for (let i = 0; i < item.children.length; ++i) {
                queue.push(item.children[i])
            }
        }
    }

    return null
}

function find_child(testcase, root, name) {
    const item = find_child_helper(root, name)
    testcase.verify(item !== null, "Item not found: " + name)
    return item
}

function click_center(testcase, item) {
    testcase.mouseClick(item, item.width / 2, item.height / 2)
}

function wheel_up(testcase, item) {
    testcase.mouseWheel(item, item.width / 2, item.height / 2, 0, 120)
}

function wheel_down(testcase, item) {
    testcase.mouseWheel(item, item.width / 2, item.height / 2, 0, -120)
}

function click_by_name(testcase, root, name) {
    const item = find_child(testcase, root, name)
    click_center(testcase, item)
}

/**
 * Waits for an object to be created and fails after a timeout.
 */
function wait_for_object(testcase, root, object_name) {
    var obj = null

    testcase.tryVerify(function() {
        obj = find_child(testcase, root, object_name)
        return obj !== null
    })

    return obj
}

/**
 * Waits for an object to be created and verifes its text property.
 */
function wait_for_text(testcase, root, object_name, expected_text) {
    var obj = find_child(testcase, root, object_name)

    testcase.tryCompare(obj, "text", expected_text)

    return obj
}

/**
 * Verifies the existence and the text property of repeater children.
 *
 * config:
 *   {
 *       label: "label_range",
 *       text: "2020 – 2029",
 *       prefix: "year_button_",
 *       start: 2020,
 *       count: 10
 *   }
 */
function verify_repeater_items(testcase, root, config) {
    for (var value = config.start_value; value < config.start_value + config.count; ++value) {
        wait_for_text(testcase, root, config.prefix + value, value.toString())
    }
}