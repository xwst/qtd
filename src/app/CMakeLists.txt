QT_ADD_EXECUTABLE(qtd
    main.cpp
)

ADD_SUBDIRECTORY(model)
INCLUDE_DIRECTORIES(model)

ADD_LIBRARY(util STATIC util.cpp)
TARGET_LINK_LIBRARIES(util PRIVATE Qt6::Sql)

QT_ADD_QML_MODULE(qtd
    URI src.app
    QML_FILES
        Main.qml
        SOURCES model/settings.cpp
        QML_FILES TagEditor.qml
        QML_FILES LineEdit.qml
        QML_FILES TagForm.qml
)

TARGET_LINK_LIBRARIES(qtd
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Widgets
    Qt6::Sql
    qmltexteditorplugin
    models
)
