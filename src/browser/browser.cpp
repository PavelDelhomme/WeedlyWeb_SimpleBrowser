#include "browser.h"
#include "browserwindow.h"
#include "downloadmanagerwidget.h"

#include <QWebEngineSettings>
#include <QFile>
#include <QDir>

using namespace Qt::StringLiterals;

Browser::Browser()
{
    m_downloadManagerWidget.setAttribute(Qt::WA_QuitOnClose, false);

    QObject::connect(
        QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested,
        &m_downloadManagerWidget, &DownloadManagerWidget::downloadRequested);
}

BrowserWindow *Browser::createHiddenWindow(bool offTheRecord)
{
    if (!offTheRecord && !m_profile) {
        const QString name = u"simplebrowser."_s + QLatin1StringView(qWebEngineChromiumVersion());
        m_profile.reset(new QWebEngineProfile(name));
        m_profile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        m_profile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
        m_profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        m_profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
        m_profile->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
        QObject::connect(m_profile.get(), &QWebEngineProfile::downloadRequested,
                         &m_downloadManagerWidget, &DownloadManagerWidget::downloadRequested);
    }
    auto profile = !offTheRecord ? m_profile.get() : QWebEngineProfile::defaultProfile();
    BrowserWindow *mainWindow = nullptr;
    try {
        mainWindow = new BrowserWindow(this, profile, false);
        if (mainWindow) {
            profile->setPersistentPermissionsPolicy(QWebEngineProfile::PersistentPermissionsPolicy::AskEveryTime);
            m_windows.append(mainWindow);
            QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
                m_windows.removeOne(mainWindow);
            });
            mainWindow->refreshFavoriteIcon(QUrl());
        }
    } catch (const std::exception& e) {
        qDebug() << "Exception lors de la création de BrowserWindow:" << e.what();
    }

    return mainWindow;
}

BrowserWindow *Browser::createWindow(bool offTheRecord)
{
    auto *mainWindow = createHiddenWindow(offTheRecord);
    mainWindow->show();
    return mainWindow;
}

BrowserWindow *Browser::createDevToolsWindow()
{
    auto profile = m_profile ? m_profile.get() : QWebEngineProfile::defaultProfile();
    auto mainWindow = new BrowserWindow(this, profile, true);
    m_windows.append(mainWindow);
    QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
        m_windows.removeOne(mainWindow);
    });
    mainWindow->show();
    return mainWindow;
}


void Browser::ensureFavoritesFileExists()
{
    QString filePath = QDir::currentPath() + "/src/favorites/favorites.json";
    QFile file(filePath);
    if (!file.exists()) {
        QDir().mkpath(QFileInfo(filePath).path());
        if (file.open(QIODevice::WriteOnly)) {
            file.write("[]");
            file.close();
        }
    }
}