import QtQuick

Rectangle {
    property alias text: text_field.text
    property alias font: text_field.font
    color: "white"
    border.color: "lightgray"
    border.width: 2
    radius: 4
    height: text_field.height + 10

    TextInput {
        id: text_field
        anchors.centerIn: parent
        width: parent.width - 6
        clip: true
        text: "empty"
    }
}
