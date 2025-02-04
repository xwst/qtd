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
    QT_ADD_EXECUTABLE(
        ${test_name}
        ${source_file}
        ../testhelpers.cpp
    )
    TARGET_COMPILE_OPTIONS(${test_name} PRIVATE -O0 --coverage)
    TARGET_LINK_LIBRARIES(${test_name}
        PRIVATE
        Qt6::Gui
        Qt6::Test
        Qt6::Sql
        gcov
        models_internal
    )
    ADD_TEST(NAME ${test_name} COMMAND ${test_name})
    ADD_DEPENDENCIES(all_tests ${test_name})
    QT_ADD_RESOURCES(
        ${test_name} "test-resources"
        PREFIX "/"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/../"
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/../resources/sql/generic/populate_database.sql"
    )
    QT_ADD_RESOURCES(
        ${test_name} "resources"
        PREFIX "/"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        FILES ${RESOURCE_FILES}
    )
ENDFUNCTION()

ADD_CUSTOM_TARGET(all_tests ALL)

FIND_PROGRAM(GCOVR NAMES gcovr)
IF(GCOVR)
    ADD_CUSTOM_TARGET(
        coverage
        ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND
        ${GCOVR}
            -r ${CMAKE_SOURCE_DIR}
            --filter ${CMAKE_SOURCE_DIR}/src
            --html-nested ${CMAKE_BINARY_DIR}/coverage.html
            --exclude-throw-branches
            --exclude-unreachable-branches
        DEPENDS all_tests
        BYPRODUCTS ${CMAKE_BINARY_DIR}/coverage.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ENDIF()


CREATE_MODEL_TEST(test_tagitemmodels testtagitemmodels.cpp)
