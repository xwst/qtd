#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QTextCursor>

class TextFormatter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QQuickTextDocument* doc MEMBER m_doc NOTIFY document_changed)
    Q_PROPERTY(int qml_cursor_position MEMBER m_qml_cursor_position)
    Q_PROPERTY(int qml_selection_start MEMBER m_qml_selection_start)
    Q_PROPERTY(int qml_selection_end MEMBER m_qml_selection_end)
public:
    explicit TextFormatter(QObject *parent = nullptr);
    Q_INVOKABLE void list_button_clicked(QTextListFormat::Style);
    Q_INVOKABLE bool change_list_indent(bool);
private:
    QQuickTextDocument* m_doc;
    int m_qml_cursor_position;
    int m_qml_selection_start;
    int m_qml_selection_end;

    QTextCursor get_textcursor();
    void remove_list(QTextList*);
    static QTextList* get_previous_list(QTextBlock, int);
    std::list<QTextBlock> get_selected_blocks();
signals:
    void document_changed();
};

#endif // TEXTFORMATTER_H
