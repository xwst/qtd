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

import QtQuick
import QtQml
import QtTest 1.0

import src.app

TestCase {
    name: "task_adapter"
    visible: true

    property var _spyNames: [
        "indexChanged", "modelChanged", "uuidChanged", "titleChanged",
        "statusChanged", "startChanged", "dueChanged", "resolveChanged",
        "plainTextChanged", "richTextChanged", "tagsChanged"
    ]

    property var _roleNames: [
        "uuidChanged", "titleChanged", "statusChanged", "startChanged",
        "dueChanged", "resolveChanged", "plainTextChanged", "richTextChanged",
        "tagsChanged"
    ]

    Instantiator {
        id: _spyInstantiator
        model: _spyNames
        delegate: SignalSpy {
            target: adapter
            signalName: modelData
        }
    }

    function _spy(signalName) {
        for (var i = 0; i < _spyInstantiator.count; i++) {
            var s = _spyInstantiator.objectAt(i);
            if (s.signalName === signalName) {
                return s;
            }
        }
        return null;
    }

    function clear_spies() {
        for (var i = 0; i < _spyInstantiator.count; i++) {
            _spyInstantiator.objectAt(i).clear();
        }
    }

    function _verify_all(expected) {
        for (var i = 0; i < _spyInstantiator.count; i++) {
            var s = _spyInstantiator.objectAt(i);
            compare(s.count, expected, s.signalName);
        }
    }

    function _verify_roles(expected) {
        for (var i = 0; i < _roleNames.length; i++) {
            compare(_spy(_roleNames[i]).count, expected, _roleNames[i]);
        }
    }

    function initTestCase() {
        adapter.index = adapter.model.index(0, 0);
        compare(_spy("modelChanged").count, 0);
        compare(_spy("indexChanged").count, 1);
        _verify_roles(1);
        clear_spies();
        compare(adapter.title, "Task 1");
        compare(adapter.plainText, "");
        compare(adapter.status, 0);
        verify(adapter.uuid.toString().length > 0);
    }

    function init() {
        clear_spies();
    }

    function cleanup() {
        adapter.model = dummyTaskModel;
        adapter.index = adapter.model.index(0, 0);
        clear_spies();
    }

    function test_set_index_same() {
        adapter.index = adapter.model.index(0, 0);
        _verify_all(0);
    }

    function test_set_index_invalid() {
        adapter.index = adapter.model.index(-1, 0);
        compare(_spy("indexChanged").count, 1);
        _verify_roles(1);
        compare(_spy("modelChanged").count, 0);
        compare(adapter.title, "");
        compare(adapter.status, 0);
    }

    function test_get_properties() {
        compare(adapter.title, "Task 1");
        compare(adapter.plainText, "");
        compare(adapter.status, 0);
        verify(adapter.richText.length > 0);
        verify(adapter.uuid.toString().length > 0);
    }

    function test_set_index_switch() {
        adapter.index = adapter.model.index(1, 0);
        compare(_spy("indexChanged").count, 1);
        _verify_roles(1);
        compare(adapter.title, "Task 2");
        compare(adapter.status, 1);
    }

    function test_set_property_title() {
        adapter.title = "Test Title";
        compare(_spy("titleChanged").count, 1);
        compare(_spy("statusChanged").count, 0);
        compare(dummyTaskModel.data(dummyTaskModel.index(0, 0), 0), "Test Title");
        adapter.title = "Task 1";
    }

    function test_set_property_status() {
        adapter.status = 1;
        compare(_spy("titleChanged").count, 0);
        compare(_spy("statusChanged").count, 1);
        compare(dummyTaskModel.data(dummyTaskModel.index(0, 0), QmlInterface.ActiveRole), 1);
        adapter.status = 0;
    }

    function test_data_changed_granular() {
        dummyTaskModel.emit_data_changed(0, [QmlInterface.ActiveRole]);

        compare(_spy("statusChanged").count, 1);
        compare(_spy("titleChanged").count, 0);
        compare(_spy("startChanged").count, 0);
        compare(_spy("dueChanged").count, 0);
        compare(_spy("resolveChanged").count, 0);
        compare(_spy("plainTextChanged").count, 0);
        compare(_spy("richTextChanged").count, 0);
        compare(_spy("tagsChanged").count, 0);
        compare(_spy("uuidChanged").count, 0);
        compare(_spy("indexChanged").count, 0);
        compare(_spy("modelChanged").count, 0);
    }

    function test_data_changed_empty_roles() {
        dummyTaskModel.emit_data_changed(0, []);

        _verify_roles(1);
        compare(_spy("indexChanged").count, 0);
        compare(_spy("modelChanged").count, 0);
    }

    function test_data_changed_other_row() {
        dummyTaskModel.emit_data_changed(1, [QmlInterface.ActiveRole]);
        _verify_all(0);
    }

    function test_set_model_null() {
        adapter.model = null;

        compare(_spy("modelChanged").count, 1);
        compare(_spy("indexChanged").count, 0);
        compare(_spy("titleChanged").count, 0);

        compare(adapter.title, "");
        compare(adapter.status, 0);

        dummyTaskModel.emit_data_changed(0, [QmlInterface.ActiveRole]);
        compare(_spy("statusChanged").count, 0);
    }
}
