#include "certificateerrordialog.h"
#include <QVBoxLayout>
#include <QSpacerItem>

CertificateErrorDialog::CertificateErrorDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Certificate Error"));
    resize(586, 169);

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(20, -1, 20, -1);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(m_iconLabel);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    verticalLayout->addWidget(m_errorLabel);

    m_infoLabel = new QLabel(this);
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setText(tr("If you wish so, you may continue with an unverified certificate. "
                            "Accepting an unverified certificate means you may not be connected with the host you tried to connect to.\n\n"
                            "Do you wish to override the security check and continue?"));
    verticalLayout->addWidget(m_infoLabel);

    verticalLayout->addItem(new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding));

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::No | QDialogButtonBox::Yes);
    verticalLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void CertificateErrorDialog::setError(const QString &error)
{
    m_errorLabel->setText(error);
}

void CertificateErrorDialog::setUrl(const QUrl &url)
{
    m_infoLabel->setText(m_infoLabel->text() + "\n\nURL: " + url.toString());
}
