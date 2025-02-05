#include "webauthdialog.h"
#include <QVBoxLayout>
#include <qwebenginewebauthuxrequest.h>

WebAuthDialog::WebAuthDialog(QWebEngineWebAuthUxRequest *request, QWidget *parent)
    : QDialog(parent), m_request(request)
{
    setWindowTitle(tr("Web Authentication"));
    resize(563, 397);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_headingLabel = new QLabel(this);
    mainLayout->addWidget(m_headingLabel);

    m_description = new QLabel(this);
    m_description->setWordWrap(true);
    mainLayout->addWidget(m_description);

    m_pinGroupBox = new QGroupBox(this);
    QVBoxLayout *pinLayout = new QVBoxLayout(m_pinGroupBox);

    QLabel *pinLabel = new QLabel(tr("PIN"), m_pinGroupBox);
    m_pinLineEdit = new QLineEdit(m_pinGroupBox);
    pinLayout->addWidget(pinLabel);
    pinLayout->addWidget(m_pinLineEdit);

    QLabel *confirmPinLabel = new QLabel(tr("Confirm PIN"), m_pinGroupBox);
    m_confirmPinLineEdit = new QLineEdit(m_pinGroupBox);
    pinLayout->addWidget(confirmPinLabel);
    pinLayout->addWidget(m_confirmPinLineEdit);

    m_pinEntryErrorLabel = new QLabel(m_pinGroupBox);
    pinLayout->addWidget(m_pinEntryErrorLabel);

    mainLayout->addWidget(m_pinGroupBox);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Retry);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void WebAuthDialog::setHeading(const QString &heading)
{
    m_headingLabel->setText(heading);
}

void WebAuthDialog::setDescription(const QString &description)
{
    m_description->setText(description);
}

QString WebAuthDialog::pin() const
{
    return m_pinLineEdit->text();
}

void WebAuthDialog::updateDisplay()
{
    switch (m_request->state()) {
        case QWebEngineWebAuthUxRequest::WebAuthUxState::SelectAccount:
            setHeading(tr("Select Account"));
            setDescription(tr("Please select an account to authenticate with."));
            break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::CollectPin:
        setHeading(tr("Enter PIN"));
        setDescription(tr("Please enter your PIN."));
        m_pinGroupBox->show();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::FinishTokenCollection:
        setHeading(tr("Finish Authentication"));
        setDescription(tr("Please complete the authentication process on your device."));
        m_pinGroupBox->hide();
        break;
    default:
        setHeading(tr("Authentication"));
        setDescription(tr("Please follow the instructions on your authenticator device."));
        m_pinGroupBox->hide();
        break;
    }
}