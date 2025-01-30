#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <QLineEdit>
#include <QCompleter>
#include <QStringList>
#include <QListWidget>

class CommandWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit CommandWidget(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void commandEntered(const QString &command);

private:
    QCompleter *m_completer;
    QStringList m_commands;

    void setupCompleter();
    void addCommand(const QString &command);
};

#endif // COMMANDWIDGET_H
