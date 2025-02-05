#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent = nullptr);
    QString username() const;
    QString password() const;
    void setMessage(const QString &message);

private:
    QLabel *m_iconLabel;
    QLabel *m_infoLabel;
    QLineEdit *m_userNameLineEdit;
    QLineEdit *m_passwordLineEdit;
    QDialogButtonBox *buttonBox;
};

#endif // PASSWORDDIALOG_H
