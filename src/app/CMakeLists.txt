# Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)

# This file is part of qtd.
#
# qtd is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# qtd is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# qtd. If not, see <https://www.gnu.org/licenses/>.

QT_ADD_EXECUTABLE(qtd
    main.cpp
    globaleventfilter.cpp
)

ADD_SUBDIRECTORY(backend)

SET_SOURCE_FILES_PROPERTIES(
    frontend/globalstyle/GlobalStyle.qml
    PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

QT_ADD_QML_MODULE(qtd
    URI src.app
    QML_FILES
        frontend/Main.qml
        SOURCES qmlinterface.cpp
        QML_FILES
            frontend/components/LineEdit.qml
            frontend/components/SelectableTreeViewDelegate.qml
            frontend/components/SelectableTreeView.qml
            frontend/components/TagEditor.qml
            frontend/components/TagForm.qml
            frontend/components/TagOutline.qml
            frontend/components/TaskPage.qml
            frontend/globalstyle/GlobalStyle.qml
)

TARGET_LINK_LIBRARIES(qtd
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Widgets
    Qt6::Sql
    qmltexteditorplugin
    backend
)
