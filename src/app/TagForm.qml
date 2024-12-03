import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    id: tag_form_container

    required property string name
    required property font control_font
    //required property list parents
    //required property int current_parent_index
    required property string parent_name
    required property color tag_color
    required property bool is_new
    required property bool has_children

    signal deleteActivated
    signal closeClicked

        anchors.fill: parent
        columnSpacing: control_font.pointSize
        rowSpacing: control_font.pointSize / 2

        columns: 2
        Label {
            id: name_label
            text: "Name:"
            font: tag_form_container.control_font
        }
        LineEdit {
            Layout.fillWidth: true
            Layout.minimumWidth: name_label.width * 2
            id: name_input
            text: tag_form_container.name
            font: tag_form_container.control_font
        }

        Label {
            text: "Parent:"
            font: tag_form_container.control_font
        }
        ComboBox {
            Layout.fillWidth: true
            id: parent_input
            model: Settings.flat_tags_model
            editable: true
            textRole: "display"
            font: tag_form_container.control_font
        }

        Label {
            text: "Color:"
            font: tag_form_container.control_font
        }
        LineEdit {
            Layout.fillWidth: true
            id: color_input
            text: tag_form_container.tag_color
            font: tag_form_container.control_font
        }

        RowLayout {
            Layout.columnSpan: 2

            DialogButtonBox {
                Layout.fillWidth: true
                alignment: Qt.AlignRight
                spacing: tag_form_container.control_font.pointSize / 2

                Button {
                    text: "Cancel"
                    leftPadding: tag_form_container.control_font.pointSize
                    rightPadding: tag_form_container.control_font.pointSize
                    font.pointSize: tag_form_container.control_font.pointSize
                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                    onClicked: tag_form_container.closeClicked()
                }

                Button {
                    text: "Save"
                    leftPadding: tag_form_container.control_font.pointSize
                    rightPadding: tag_form_container.control_font.pointSize
                    font.pointSize: tag_form_container.control_font.pointSize
                    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
                }

                DelayButton {
                        id: delete_button

                        delay: tag_form_container.has_children ? 0 : 700
                        leftPadding: tag_form_container.control_font.pointSize
                        rightPadding: tag_form_container.control_font.pointSize
                        text: "Delete"
                        font.pointSize: tag_form_container.control_font.pointSize
                        palette.buttonText: "red"
                        DialogButtonBox.buttonRole: DialogButtonBox.ResetRole
                        onActivated: tag_form_container.deleteActivated()
                }
            }
        }
    }


