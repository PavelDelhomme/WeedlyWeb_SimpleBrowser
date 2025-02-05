#ifndef CERTIFICATEERRORDIALOG_H
#define CERTIFICATEERRORDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>

class CertificateErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CertificateErrorDialog(QWidget *parent = nullptr);
    void setError(const QString &error);
    void setUrl(const QUrl &url);

private:
    QLabel *m_iconLabel;
    QLabel *m_errorLabel;
    QLabel *m_infoLabel;
    QDialogButtonBox *buttonBox;
};

#endif // CERTIFICATEERRORDIALOG_H
