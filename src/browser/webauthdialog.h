#ifndef WEBAUTHDIALOG_H
#define WEBAUTHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QWebEngineWebAuthUxRequest>

class WebAuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WebAuthDialog(QWebEngineWebAuthUxRequest *request, QWidget *parent = nullptr);
    void setHeading(const QString &heading);
    void setDescription(const QString &description);
    QString pin() const;
    void updateDisplay();

private:
    QWebEngineWebAuthUxRequest *m_request;
    QLabel *m_headingLabel;
    QLabel *m_description;
    QGroupBox *m_pinGroupBox;
    QLineEdit *m_pinLineEdit;
    QLineEdit *m_confirmPinLineEdit;
    QLabel *m_pinEntryErrorLabel;
    QDialogButtonBox *buttonBox;
};

#endif // WEBAUTHDIALOG_H
