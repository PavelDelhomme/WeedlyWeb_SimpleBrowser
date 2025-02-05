#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QFile>
#include <QDir>


using namespace Qt::StringLiterals;

QUrl commandLineUrlArgument()
{
    const QStringList args = QCoreApplication::arguments();
    for (const QString &arg : args.mid(1)) {
        if (!arg.startsWith(u'-'))
            return QUrl::fromUserInput(arg);
    }
    return QUrl(u"chrome://qt"_s);
}

int main(int argc, char **argv)
{
    qputenv("QT_LOGGING_RULES", "qt.webenginecontext.debug=true");

    QCoreApplication::setOrganizationName("QtExamples");

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(u":AppLogoColor.png"_s));
    QLoggingCategory::setFilterRules(u"qt.webenginecontext.debug=true"_s);

    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(
            QWebEngineSettings::ScreenCaptureEnabled, true);

    QUrl url = commandLineUrlArgument();

    QDir().mkpath(QDir::currentPath() + "/src/favorites");
    QFile file(QDir::currentPath() + "/src/favorites/favorites.json");
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            file.write("[]");
            file.close();
        }
    }

    Browser browser;
    BrowserWindow *window = browser.createHiddenWindow();
    window->tabWidget()->setUrl(url);
    window->show();
    return app.exec();
}
