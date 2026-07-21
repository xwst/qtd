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

SET(QML_COMPONENT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

FUNCTION(CREATE_QML_COMPONENT_TEST)
    SET(options "")
    SET(oneValueArgs TEST_NAME QML_DIR SETUP)
    SET(multiValueArgs LIBS)
    CMAKE_PARSE_ARGUMENTS(PARSE_ARGV 0 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

    SET(runner "${CMAKE_CURRENT_BINARY_DIR}/${arg_TEST_NAME}_runner.cpp")

    SET(TEST_NAME "${arg_TEST_NAME}")
    SET(QML_DIR "${arg_QML_DIR}")

    IF(arg_SETUP)
        CONFIGURE_FILE(
            "${QML_COMPONENT_TEST_DIR}/test_runner_setup.cpp.in"
            "${runner}"
            @ONLY
        )
        ADD_EXECUTABLE(${arg_TEST_NAME} "${runner}" "${PROJECT_SOURCE_DIR}/test/backend/testmodelwrappers.h")
    ELSE()
        CONFIGURE_FILE(
            "${QML_COMPONENT_TEST_DIR}/test_runner_simple.cpp.in"
            "${runner}"
            @ONLY
        )
        ADD_EXECUTABLE(${arg_TEST_NAME} "${runner}")
    ENDIF()

    TARGET_INCLUDE_DIRECTORIES(${arg_TEST_NAME}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/app"
        PRIVATE "${PROJECT_SOURCE_DIR}/src/app/backend"
        PRIVATE "${PROJECT_SOURCE_DIR}/test"
    )

    TARGET_LINK_LIBRARIES(${arg_TEST_NAME} PRIVATE
        Qt6::QuickTest
        Qt6::Qml
        Qt6::Gui
        app_library
        app_libraryplugin
        app_libraryplugin_init
        qmltexteditorplugin
        datetimepickerplugin
        datetimepickerplugin_init
        ${arg_LIBS}
    )

    ADD_TEST(NAME ${arg_TEST_NAME} COMMAND ${arg_TEST_NAME})
    SET_TESTS_PROPERTIES(${arg_TEST_NAME} PROPERTIES LABELS "frontend-test")
    ADD_DEPENDENCIES(all_tests ${arg_TEST_NAME})
ENDFUNCTION()


CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_taskadapter
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/taskadapter"
    SETUP     YES
)

CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_tag_editor
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/tag_editor"
    SETUP     YES
)

CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_text_editor
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/text_editor"
)

CREATE_QML_COMPONENT_TEST(
    TEST_NAME test_choice_button
    QML_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/choice_button"
)

ADD_SUBDIRECTORY(datetimepicker)
