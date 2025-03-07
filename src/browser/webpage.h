#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineCertificateError>
#include <QWebEngineDesktopMediaRequest>

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);

signals:
    void createCertificateErrorDialog(QWebEngineCertificateError error);

private slots:
    void handleCertificateError(QWebEngineCertificateError error);
    void handleSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection);
    void handleDesktopMediaRequest(const QWebEngineDesktopMediaRequest &request);
};

#endif // WEBPAGE_H
