#include "passworddialog.h"
#include <QGridLayout>

PasswordDialog::PasswordDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Authentication Required"));
    resize(399, 148);

    QGridLayout *gridLayout = new QGridLayout(this);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(m_iconLabel, 0, 0, 1, 1);

    m_infoLabel = new QLabel(this);
    m_infoLabel->setWordWrap(true);
    gridLayout->addWidget(m_infoLabel, 0, 1, 1, 1);

    QLabel *userLabel = new QLabel(tr("Username:"), this);
    gridLayout->addWidget(userLabel, 1, 0, 1, 1);

    m_userNameLineEdit = new QLineEdit(this);
    gridLayout->addWidget(m_userNameLineEdit, 1, 1, 1, 1);

    QLabel *passwordLabel = new QLabel(tr("Password:"), this);
    gridLayout->addWidget(passwordLabel, 2, 0, 1, 1);

    m_passwordLineEdit = new QLineEdit(this);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    gridLayout->addWidget(m_passwordLineEdit, 2, 1, 1, 1);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, 3, 0, 1, 2);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString PasswordDialog::username() const
{
    return m_userNameLineEdit->text();
}

QString PasswordDialog::password() const
{
    return m_passwordLineEdit->text();
}

void PasswordDialog::setMessage(const QString &message)
{
    m_infoLabel->setText(message);
}
