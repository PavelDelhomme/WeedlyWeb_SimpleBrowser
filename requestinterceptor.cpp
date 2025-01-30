#include "requestinterceptor.h"
#include <QUuid>

RequestInterceptor::RequestInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent), m_requestCounter(0)
{
}

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QString id = QUuid::createUuid().toString();
    QNetworkRequest request(info.requestUrl());

    m_interceptedRequests[id] = request;
    emit requestIntercepted(id, request);

    m_requestCounter++;
}

QMap<QString, QNetworkRequest> RequestInterceptor::getInterceptedRequests() const
{
    return m_interceptedRequests;
}

void RequestInterceptor::modifyRequest(const QString &id, const QNetworkRequest &newRequest)
{
    if (m_interceptedRequests.contains(id)) {
        m_interceptedRequests[id] = newRequest;
    }
}
