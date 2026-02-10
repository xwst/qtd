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


FUNCTION(create_model_test test_name source_file)
    SET(options QML)
    SET(oneValueArgs TEST_NAME)
    SET(multiValueArgs SOURCES)
    CMAKE_PARSE_ARGUMENTS(
        PARSE_ARGV 0 arg "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )
    QT_ADD_EXECUTABLE(
        ${arg_TEST_NAME}
        ${arg_SOURCES}
        ../testhelpers.cpp
        persistedtreeitemmodelstestbase.cpp
    )
    IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        TARGET_COMPILE_OPTIONS(${arg_TEST_NAME} PRIVATE -O0 --coverage)
        TARGET_LINK_LIBRARIES(${arg_TEST_NAME} PRIVATE gcov)
        TARGET_LINK_OPTIONS(${arg_TEST_NAME} PRIVATE --coverage)
    ENDIF()
    TARGET_LINK_LIBRARIES(${arg_TEST_NAME}
        PRIVATE
        Qt6::Test
        Qt6::Sql
        Qt6::Gui
        backend_internal
    )
    IF (${arg_QML})
        TARGET_LINK_LIBRARIES(${arg_TEST_NAME} PRIVATE Qt6::Quick)
    ENDIF()
    ADD_TEST(NAME ${arg_TEST_NAME} COMMAND ${arg_TEST_NAME})
    ADD_DEPENDENCIES(all_tests ${arg_TEST_NAME})
    QT_ADD_RESOURCES(
        ${arg_TEST_NAME} "test-resources"
        PREFIX "/"
	BASE "${PROJECT_SOURCE_DIR}/test"
        FILES "${PROJECT_SOURCE_DIR}/test/resources/sql/generic/populate_database.sql"
    )
    QT_ADD_RESOURCES(
        ${arg_TEST_NAME} "resources"
        PREFIX "/"
	BASE "${PROJECT_SOURCE_DIR}"
        FILES ${RESOURCE_FILES}
    )
ENDFUNCTION()

ADD_CUSTOM_TARGET(all_tests ALL)

FIND_PROGRAM(GCOVR NAMES gcovr)
IF(GCOVR AND (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"))
    ADD_CUSTOM_TARGET(
        coverage
        ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND
        ${GCOVR}
            -r ${CMAKE_SOURCE_DIR}
            --filter ${CMAKE_SOURCE_DIR}/src
            --html-nested ${CMAKE_BINARY_DIR}/gcovr/coverage.html
            --exclude-throw-branches
            --exclude-unreachable-branches
        DEPENDS all_tests
        BYPRODUCTS ${CMAKE_BINARY_DIR}/gcovr/coverage.html
    )
    ADD_CUSTOM_TARGET(
        verify-test-coverage
        ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND
        ${GCOVR}
            -r ${CMAKE_SOURCE_DIR}
            --filter ${CMAKE_SOURCE_DIR}/src
            --fail-under-line 90
            --fail-under-function 90
        DEPENDS all_tests
    )
ENDIF()

CREATE_MODEL_TEST(TEST_NAME test_ids                   SOURCES testqtdid.cpp)
CREATE_MODEL_TEST(TEST_NAME test_treenodes             SOURCES testtreenodes.cpp)
CREATE_MODEL_TEST(TEST_NAME test_treeitemmodel         SOURCES testtreeitemmodel.cpp)
CREATE_MODEL_TEST(TEST_NAME test_tagitemmodels         SOURCES testtagitemmodels.cpp)
CREATE_MODEL_TEST(TEST_NAME test_taskitemmodels        SOURCES testtaskitemmodels.cpp)
CREATE_MODEL_TEST(TEST_NAME test_filteredtaskitemmodel SOURCES testfilteredtaskitemmodel.cpp)
CREATE_MODEL_TEST(TEST_NAME test_filteredtagitemmodel  SOURCES testfilteredtagitemmodel.cpp)
CREATE_MODEL_TEST(
    TEST_NAME test_qmlinterface
    QML
    SOURCES testqmlinterface.cpp
            ../../src/app/qmlinterface.cpp
            ../../src/app/globaleventfilter.cpp
)
