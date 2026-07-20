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

ADD_CUSTOM_TARGET(all_tests ALL)

ADD_SUBDIRECTORY(backend/)
ADD_SUBDIRECTORY(frontend/component/)
ADD_SUBDIRECTORY(frontend/journey/)

FIND_PROGRAM(GCOVR NAMES gcovr)
IF(GCOVR AND (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"))
    ADD_CUSTOM_TARGET(
        coverage
        find ${CMAKE_BINARY_DIR} -name "*.gcda" -type f -delete
        COMMAND
        QT_QPA_PLATFORM=offscreen ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
        COMMAND
        ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/gcovr
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
        find ${CMAKE_BINARY_DIR} -name "*.gcda" -type f -delete
        COMMAND
        QT_QPA_PLATFORM=offscreen ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
        COMMAND
        ${GCOVR}
            -r ${CMAKE_SOURCE_DIR}
            --filter ${CMAKE_SOURCE_DIR}/src
            --fail-under-line 90
            --fail-under-function 90
        DEPENDS all_tests
    )
ENDIF()
