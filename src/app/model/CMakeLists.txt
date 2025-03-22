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
    APPEND model_files
    tagitemmodel.cpp
    flatteningproxymodel.cpp
    tag.cpp
    treenode.cpp
    treeitemmodel.cpp
    uniquedataitem.cpp
    task.cpp
    taskitemmodel.cpp
)

LIST(
    APPEND model_dependencies
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    util
)

QT_ADD_LIBRARY(models STATIC ${model_files})
TARGET_LINK_LIBRARIES(models PRIVATE ${model_dependencies})
TARGET_COMPILE_OPTIONS(models PRIVATE -O3)

QT_ADD_LIBRARY(models_internal STATIC ${model_files})
TARGET_LINK_LIBRARIES(models_internal PRIVATE ${model_dependencies})
TARGET_COMPILE_OPTIONS(models_internal PRIVATE -O0 --coverage)
