QT_ADD_LIBRARY(
    models STATIC
    TagItemModel.cpp flatteningproxymodel.cpp
    tag.cpp
)

TARGET_LINK_LIBRARIES(models
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    util
)
