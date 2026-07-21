# Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)

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

QT_ADD_EXECUTABLE(
    test_datetimeparser
    ../../../../src/datetimepicker/datetimeparser.cpp
    ../../../../src/datetimepicker/datetimeparser.h
    testdatetimeparser.cpp
)
TARGET_INCLUDE_DIRECTORIES(test_datetimeparser PRIVATE ../../../../src/datetimepicker/)
IF(QTD_ENABLE_COVERAGE)
    TARGET_COMPILE_OPTIONS(test_datetimeparser PRIVATE -O0 --coverage)
    TARGET_LINK_OPTIONS(test_datetimeparser PRIVATE --coverage)
    IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        TARGET_LINK_LIBRARIES(test_datetimeparser PRIVATE gcov)
    ENDIF()
ENDIF()
TARGET_LINK_LIBRARIES(test_datetimeparser
    PRIVATE
    Qt6::Test
    Qt6::Core
    Qt6::Quick
)
IF(MSVC)
    SET_TARGET_PROPERTIES(test_datetimeparser PROPERTIES WIN32_EXECUTABLE FALSE)
ENDIF()
ADD_TEST(NAME test_datetimeparser COMMAND test_datetimeparser)
SET_TESTS_PROPERTIES(test_datetimeparser PROPERTIES LABELS "frontend-test")
ADD_DEPENDENCIES(all_tests test_datetimeparser)

CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_datetime_picker
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/datetime_picker"
)
CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_datetime_picker_panel
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/datetime_picker_panel"
)
CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_month_picker
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/month_picker"
)
CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_year_picker
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/year_picker"
)
CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_vertical_spinbox
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/vertical_spinbox"
)
