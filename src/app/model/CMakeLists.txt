QT_ADD_LIBRARY(
    models STATIC
    tagitemmodel.cpp flatteningproxymodel.cpp
    tag.cpp
    treenode.cpp
    treeitemmodel.cpp
    uniquedataitem.cpp
    task.cpp
    taskitemmodel.cpp
)

TARGET_LINK_LIBRARIES(models
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    util
)
