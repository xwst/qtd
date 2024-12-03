import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property alias model: control.model
    property alias clip: control.popup.clip
    property alias textRole: control.textRole
    property alias valueRole: control.valueRole
    property alias currentValue: control.currentValue
    signal clicked

    implicitWidth: 50
    height: button.height
    Button {
        id: button
        text: control.currentText
        width: parent.width - control.width
        onClicked: parent.clicked()
    }
    ComboBox {
        id: control
        anchors.left: button.right
        displayText: '▼'
        implicitWidth: 20
        popup: Popup {
            y: parent.height
            x: -button.width
            width: button.width + parent.width
            contentItem: ListView {
                implicitHeight: contentHeight
                model: control.delegateModel
                currentIndex: control.highlightedIndex
            }
        }
        onActivated: button.clicked()
    }
}
