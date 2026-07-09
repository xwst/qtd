# Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)

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

ADD_DEFINITIONS(-DQUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
ADD_EXECUTABLE(test_components test_components.cpp ../../backend/testmodelwrappers.h)
ADD_TEST(NAME test_components COMMAND test_components)
SET_PROPERTY(TEST test_components PROPERTY LABELS "frontend-test")

TARGET_LINK_LIBRARIES(test_components
    PRIVATE
    Qt6::QuickTest
    Qt6::Qml
    Qt6::Gui
    app_library
    app_libraryplugin
    app_libraryplugin_init
    qmltexteditorplugin
    datetimepickerplugin
    datetimepickerplugin_init
)

QT_ADD_RESOURCES(
    test_components "resources"
    PREFIX "/"
    BASE "${PROJECT_SOURCE_DIR}"
    FILES ${RESOURCE_FILES}
)
ADD_DEPENDENCIES(all_tests test_components)

ADD_SUBDIRECTORY(datetimepicker)
