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
    TARGET_LINK_LIBRARIES(${test_name}
        PRIVATE
        Qt6::Gui
        Qt6::Test
        Qt6::Sql
        models
    )
    ADD_TEST(NAME ${test_name} COMMAND ${test_name})
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


CREATE_MODEL_TEST(test_tagitemmodels testtagitemmodels.cpp)
