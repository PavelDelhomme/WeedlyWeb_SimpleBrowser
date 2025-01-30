#ifndef REQUESTINTERCEPTOR_H
#define REQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMap>

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit RequestInterceptor(QObject *parent = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    QMap<QString, QNetworkRequest> getInterceptedRequests() const;
    void modifyRequest(const QString &id, const QNetworkRequest &newRequest);

signals:
    void requestIntercepted(const QString &id, const QNetworkRequest &request);

private:
    QMap<QString, QNetworkRequest> m_interceptedRequests;
    int m_requestCounter;
};

#endif // REQUESTINTERCEPTOR_H
