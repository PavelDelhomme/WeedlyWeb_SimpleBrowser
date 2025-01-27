// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QWebEnginePage>
#include <QMenu>
#include <QAction>
#include <QJsonArray>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QComboBox>
#include <QTreeWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>


QT_BEGIN_NAMESPACE
class QLineEdit;
class QProgressBar;
QT_END_NAMESPACE

class Browser;
class TabWidget;
class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(Browser *browser, QWebEngineProfile *profile,
                           bool forDevTools = false);
    QSize sizeHint() const override;
    TabWidget *tabWidget() const;
    WebView *currentTab() const;
    Browser *browser() { return m_browser; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void handleNewWindowTriggered();
    void handleNewIncognitoWindowTriggered();
    void handleFileOpenTriggered();
    void handleFindActionTriggered();
    void handleShowWindowTriggered();
    void handleWebViewLoadProgress(int);
    void handleWebViewTitleChanged(const QString &title);
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void handleDevToolsRequested(QWebEnginePage *source);
    void handleFindTextFinished(const QWebEngineFindTextResult &result);
    void handleFavActionTriggered();
    void showFavoritesManager();
    void saveFavoritesFromTree(QTreeWidget *tree);
    void updateFavoriteIcon(const QUrl &url);


private:
    QMenu *createFileMenu(TabWidget *tabWidget);
    QMenu *createEditMenu();
    QMenu *createViewMenu(QToolBar *toolBar);
    QMenu *createWindowMenu(TabWidget *tabWidget);
    QMenu *createHelpMenu();
    QToolBar *createToolBar();

private:
    Browser *m_browser;
    QWebEngineProfile *m_profile;
    TabWidget *m_tabWidget;
    QProgressBar *m_progressBar = nullptr;
    QAction *m_historyBackAction = nullptr;
    QAction *m_historyForwardAction = nullptr;
    QAction *m_stopAction = nullptr;
    QAction *m_reloadAction = nullptr;
    QAction *m_stopReloadAction = nullptr;
    QLineEdit *m_urlLineEdit = nullptr;
    QAction *m_favAction = nullptr;
    QString m_lastSearch;
    QMenu *m_settingsMenu = nullptr;
    QAction *m_settingsAction = nullptr;
    QToolBar *m_toolbar = nullptr;
    QToolBar *m_favoritesBar = nullptr;
    QAction *m_moreFavoritesAction = nullptr;
    QMenu *m_favoritesMenu = nullptr;
    QVector<QPair<QString, QString>> m_favorites;

    bool isFavorite(const QUrl &url);

    // Fonctions
    void setupFavoritesBar();
    void setupFavoritesMenu();
    void addCurrentPageToFavorites();
    void addFavorite(const QString &name, const QString &url);
    void loadFavoritesToBar();
    void openFavorite(const QUrl &url);
    void saveFavorite(const QUrl &url, const QString &title);
    void loadFavorites();
    void loadFavoritesToBarRecursive(const QJsonArray& array, QWidget* parent);

    void deleteFavorite(const QUrl &url);
    void updateFavorite(const QUrl &oldUrl, const QString &newTitle, const QUrl &newUrl, const QString &newFolder);
    void showFavoriteContextMenu(const QPoint &pos, const QString &title, const QUrl &url);
    void editFavorite(const QString &oldTitle, const QUrl &oldUrl);
    QString saveFavicon(const QByteArray &data, const QUrl &url);
    void updateFaviconForFavorite(const QUrl &url, const QString &faviconPath);
    void handleWebViewLoadFinished(bool ok);
};

#endif // BROWSERWINDOW_H
