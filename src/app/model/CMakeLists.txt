LIST(
    APPEND model_files
    TagItemModel.cpp
    flatteningproxymodel.cpp
    tag.cpp
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
