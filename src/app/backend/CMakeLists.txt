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

LIST(
    APPEND backend_files
    dataitems/qtdid.cpp
    dataitems/tag.cpp
    dataitems/qtditemdatarole.cpp
    dataitems/task.cpp
    dataitems/treenode.cpp
    models/filteredtagitemmodel.cpp
    models/filteredtaskitemmodel.cpp
    models/flatteningproxymodel.cpp
    models/mainpagemodelfilter.cpp
    models/tagitemmodel.cpp
    models/taskitemmodel.cpp
    models/treeitemmodel.cpp
    utils/initialize.cpp
    utils/modeliteration.cpp
    utils/query_utilities.cpp
    repositories/tagrepository.cpp
    repositories/taskrepository.cpp
    repositories/transactionalrepository.cpp
)

LIST(
    APPEND backend_dependencies
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Concurrent
)

QT_ADD_LIBRARY(backend STATIC ${backend_files})
TARGET_LINK_LIBRARIES(backend PRIVATE ${backend_dependencies})
TARGET_COMPILE_OPTIONS(backend PRIVATE -O3)

QT_ADD_LIBRARY(backend_internal STATIC ${backend_files})
TARGET_LINK_LIBRARIES(backend_internal PRIVATE ${backend_dependencies})
IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    TARGET_COMPILE_OPTIONS(backend_internal PRIVATE -O0 --coverage)
ENDIF()
