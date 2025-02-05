#include "cveanalyzer.h"
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>

QStringList CVEAnalyzer::detectCVEs(const QString &html)
{
    QStringList cves;
    QRegularExpression re("CVE-\\d{4}-\\d{4}-\\d{4,7}");
    QRegularExpression
}
