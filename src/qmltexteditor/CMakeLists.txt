QT_ADD_LIBRARY(qmltexteditor STATIC)

QT_ADD_QML_MODULE(qmltexteditor
    URI src.qmltexteditor
    VERSION 1.0
    QML_FILES
        ChoiceButton.qml
        TextEditor.qml
    SOURCES
        textformatter.h textformatter.cpp
)

TARGET_LINK_LIBRARIES(qmltexteditor PRIVATE Qt6::Quick)
