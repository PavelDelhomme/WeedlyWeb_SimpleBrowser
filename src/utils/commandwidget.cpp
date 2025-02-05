#include "commandwidget.h"
#include <QKeyEvent>
#include <QListWidget>
#include <QStringListModel>

CommandWidget::CommandWidget(QWidget *parent) : QLineEdit(parent)
{
    setPlaceholderText("Entrez une commande...");
    hide();

    m_commands << "/cvec detect" << "/request GET" << "/request POST" << "/analyze";
    setupCompleter();
}

void CommandWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        emit commandEntered(text());
        clear();
        hide();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void CommandWidget::setupCompleter()
{
    m_completer = new QCompleter(m_commands, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchStartsWith);
    setCompleter(m_completer);
}

void CommandWidget::addCommand(const QString &command)
{
    if (!m_commands.contains(command)) {
        m_commands << command;
        m_completer->setModel(new QStringListModel(m_commands, m_completer));
    }
}
