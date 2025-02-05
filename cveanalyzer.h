#ifndef CVEANALYZER_H
#define CVEANALYZER_H

#include <QString>
#include <QStringList>

class CVEAnalyzer
{
public:
    static QStringList detectCVEs(const QString &html);
    static void displayCVEResults(const QStringList &cves);
};

#endif // CVEANALYZER_H
