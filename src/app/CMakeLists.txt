QT_ADD_EXECUTABLE(qtd
    main.cpp
)

INCLUDE_DIRECTORIES(model)

QT_ADD_QML_MODULE(qtd
    URI src.app
    QML_FILES
        Main.qml
        SOURCES model/TagItemModel.h model/TagItemModel.cpp
        SOURCES model/tag.h model/tag.cpp
        SOURCES model/settings.h model/settings.cpp
        QML_FILES TagEditor.qml
        QML_FILES LineEdit.qml
        QML_FILES TagForm.qml
        SOURCES model/flatteningproxymodel.h model/flatteningproxymodel.cpp
        SOURCES util.h util.cpp
)

TARGET_LINK_LIBRARIES(qtd
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Widgets
    Qt6::Sql
    qmltexteditorplugin
)
