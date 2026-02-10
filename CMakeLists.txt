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


CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(qtd VERSION 0.1 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
IF(MSVC)
    ADD_COMPILE_OPTIONS(/W4 /WX $<$<CONFIG:RELEASE>:/O2> /wd4702)
ELSE()
    ADD_COMPILE_OPTIONS(-Wall -Wextra -Wpedantic -Werror $<$<CONFIG:RELEASE>:-O2>)
ENDIF()

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

INCLUDE(CTest)
ENABLE_TESTING()

FIND_PACKAGE(
    Qt6
    REQUIRED COMPONENTS
        Widgets
        Core
        Gui
        Quick
        QuickControls2
        Sql
        Test
        QuickTest
        Qml
        Concurrent
)
INCLUDE(CTest)

QT_STANDARD_PROJECT_SETUP(REQUIRES 6.7)


FILE(GLOB_RECURSE RESOURCE_FILES
     LIST_DIRECTORIES false
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.sql")

INCLUDE_DIRECTORIES(src/app/backend/)
ADD_SUBDIRECTORY(resources/fonts)
ADD_SUBDIRECTORY(src/qmltexteditor)
ADD_SUBDIRECTORY(src/app)
ADD_SUBDIRECTORY(test/backend)
ADD_SUBDIRECTORY(test/frontend/component/)
ADD_SUBDIRECTORY(test/frontend/journey/)

QT_ADD_RESOURCES(
    qtd "resources"
    PREFIX "/"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${RESOURCE_FILES}
)
