QT_ADD_LIBRARY(
    models STATIC
    TagItemModel.cpp flatteningproxymodel.cpp
    tag.cpp
    treeitem.h treeitem.cpp
    uniquedataitem.h uniquedataitem.cpp
    task.h task.cpp
    taskitemmodel.h taskitemmodel.cpp
)

TARGET_LINK_LIBRARIES(models
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    util
)
