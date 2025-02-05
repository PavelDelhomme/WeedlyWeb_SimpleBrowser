#include "cveanalyzer.h"
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>

QStringList CVEAnalyzer::detectCVEs(const QString &html)
{
    QStringList cves;
    QRegularExpression re("CVE-\\d{4}-\\d{4}-\\d{4,7}");
    QRegularExpressionMatchIterator i = re.globalMatch(html);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        cves << match.captured(0);
    }
    return cves;
}

void CVEAnalyzer::displayCVEResults(const QStringList &cves)
{
    if (cves.isEmpty()) {
        qDebug() << "Aucun CVE détecté.";
    } else {
        qDebug() << "CVEs détectés : ";
        for (const QString &cve : cves) {
            qDebug() << cve;
        }
    }
}
