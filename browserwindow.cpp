// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "browser.h"
#include "browserwindow.h"
#include "downloadmanagerwidget.h"
#include "tabwidget.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

using namespace Qt::StringLiterals;

BrowserWindow::BrowserWindow(Browser *browser, QWebEngineProfile *profile, bool forDevTools)
    : m_browser(browser)
    , m_profile(profile)
    , m_tabWidget(new TabWidget(profile, this))
    , m_favoritesBar(new QToolBar(tr("Favoris"), this))  // Initialisation de la barre de favoris
    , m_progressBar(new QProgressBar(this))  // Initialisation de la barre de progression
    , m_toolbar(createToolBar())  // Initialisation de la barre de navigation
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    if (!forDevTools) {
        m_progressBar = new QProgressBar(this);

        // QToolBar *toolbar = createToolBar();
        // addToolBar(toolbar);
        addToolBar(m_toolbar);

        // Barre de favoris ajoutée avant les onglets
        //m_favoritesBar = new QToolBar(tr("Favris"));
        m_favoritesBar->setMovable(false);
        //addToolBar(m_favoritesBar);

        setupFavoritesBar();
        loadFavoritesToBar();

        menuBar()->addMenu(createFileMenu(m_tabWidget));
        menuBar()->addMenu(createEditMenu());
        //menuBar()->addMenu(createViewMenu(toolbar));
        menuBar()->addMenu(createViewMenu(m_toolbar));
        menuBar()->addMenu(createWindowMenu(m_tabWidget));
        menuBar()->addMenu(createHelpMenu());
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    if (!forDevTools) {
        addToolBarBreak();

        m_progressBar->setMaximumHeight(1);
        m_progressBar->setTextVisible(false);
        m_progressBar->setStyleSheet(u"QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"_s);

        layout->addWidget(m_progressBar);
    }

    layout->addWidget(m_toolbar);

    layout->addWidget(m_favoritesBar);

    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    if (!forDevTools) {
        connect(m_tabWidget, &TabWidget::linkHovered, [this](const QString& url) {
            statusBar()->showMessage(url);
        });
        connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
        connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);
        connect(m_tabWidget, &TabWidget::urlChanged, [this](const QUrl &url) {
            m_urlLineEdit->setText(url.toDisplayString());
        });
        connect(m_tabWidget, &TabWidget::favIconChanged, m_favAction, &QAction::setIcon);
        connect(m_tabWidget, &TabWidget::devToolsRequested, this, &BrowserWindow::handleDevToolsRequested);
        connect(m_urlLineEdit, &QLineEdit::returnPressed, [this]() {
            m_tabWidget->setUrl(QUrl::fromUserInput(m_urlLineEdit->text()));
        });
        connect(m_tabWidget, &TabWidget::findTextFinished, this, &BrowserWindow::handleFindTextFinished);

        QAction *focusUrlLineEditAction = new QAction(this);
        addAction(focusUrlLineEditAction);
        focusUrlLineEditAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
        connect(focusUrlLineEditAction, &QAction::triggered, this, [this] () {
            m_urlLineEdit->setFocus(Qt::ShortcutFocusReason);
        });
    }

    handleWebViewTitleChanged(QString());
    m_tabWidget->createTab();
}

QSize BrowserWindow::sizeHint() const
{
    QRect desktopRect = QApplication::primaryScreen()->geometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}

QMenu *BrowserWindow::createFileMenu(TabWidget *tabWidget)
{
    QMenu *fileMenu = new QMenu(tr("&File"));
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    fileMenu->addAction(tr("&New Window"), QKeySequence::New, this, &BrowserWindow::handleNewWindowTriggered);
#else
    fileMenu->addAction(tr("&New Window"), this, &BrowserWindow::handleNewWindowTriggered, QKeySequence::New);
#endif
    fileMenu->addAction(tr("New &Incognito Window"), this, &BrowserWindow::handleNewIncognitoWindowTriggered);

    QAction *newTabAction = new QAction(tr("New &Tab"), this);
    newTabAction->setShortcuts(QKeySequence::AddTab);
    connect(newTabAction, &QAction::triggered, this, [this]() {
        m_tabWidget->createTab();
        m_urlLineEdit->setFocus();
    });
    fileMenu->addAction(newTabAction);

#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    fileMenu->addAction(tr("&Open File..."), QKeySequence::Open, this, &BrowserWindow::handleFileOpenTriggered);
#else
    fileMenu->addAction(tr("&Open File..."), this, &BrowserWindow::handleFileOpenTriggered, QKeySequence::Open);
#endif
    fileMenu->addSeparator();

    QAction *closeTabAction = new QAction(tr("&Close Tab"), this);
    closeTabAction->setShortcuts(QKeySequence::Close);
    connect(closeTabAction, &QAction::triggered, [tabWidget]() {
        tabWidget->closeTab(tabWidget->currentIndex());
    });
    fileMenu->addAction(closeTabAction);

    QAction *closeAction = new QAction(tr("&Quit"),this);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(closeAction);

    connect(fileMenu, &QMenu::aboutToShow, [this, closeAction]() {
        if (m_browser->windows().count() == 1)
            closeAction->setText(tr("&Quit"));
        else
            closeAction->setText(tr("&Close Window"));
    });
    return fileMenu;
}

QMenu *BrowserWindow::createEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"));
    QAction *findAction = editMenu->addAction(tr("&Find"));
    findAction->setShortcuts(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &BrowserWindow::handleFindActionTriggered);

    QAction *findNextAction = editMenu->addAction(tr("Find &Next"));
    findNextAction->setShortcut(QKeySequence::FindNext);
    connect(findNextAction, &QAction::triggered, [this]() {
        if (!currentTab() || m_lastSearch.isEmpty())
            return;
        currentTab()->findText(m_lastSearch);
    });

    QAction *findPreviousAction = editMenu->addAction(tr("Find &Previous"));
    findPreviousAction->setShortcut(QKeySequence::FindPrevious);
    connect(findPreviousAction, &QAction::triggered, [this]() {
        if (!currentTab() || m_lastSearch.isEmpty())
            return;
        currentTab()->findText(m_lastSearch, QWebEnginePage::FindBackward);
    });

    return editMenu;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolbar)
{
    QMenu *viewMenu = new QMenu(tr("&View"));
    m_stopAction = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stopAction->setShortcuts(shortcuts);
    connect(m_stopAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Stop);
    });

    m_reloadAction = viewMenu->addAction(tr("Reload Page"));
    m_reloadAction->setShortcuts(QKeySequence::Refresh);
    connect(m_reloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Reload);
    });

    QAction *zoomIn = viewMenu->addAction(tr("Zoom &In"));
    zoomIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    });

    QAction *zoomOut = viewMenu->addAction(tr("Zoom &Out"));
    zoomOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    });

    QAction *resetZoom = viewMenu->addAction(tr("Reset &Zoom"));
    resetZoom->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoom, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(1.0);
    });


    viewMenu->addSeparator();
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+|"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible()) {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);

    QAction *viewStatusbarAction = new QAction(tr("Hide Status Bar"), this);
    viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    connect(viewStatusbarAction, &QAction::triggered, [this, viewStatusbarAction]() {
        if (statusBar()->isVisible()) {
            viewStatusbarAction->setText(tr("Show Status Bar"));
            statusBar()->close();
        } else {
            viewStatusbarAction->setText(tr("Hide Status Bar"));
            statusBar()->show();
        }
    });
    viewMenu->addAction(viewStatusbarAction);
    return viewMenu;
}

QMenu *BrowserWindow::createWindowMenu(TabWidget *tabWidget)
{
    QMenu *menu = new QMenu(tr("&Window"));

    QAction *nextTabAction = new QAction(tr("Show Next Tab"), this);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
    nextTabAction->setShortcuts(shortcuts);
    connect(nextTabAction, &QAction::triggered, tabWidget, &TabWidget::nextTab);

    QAction *previousTabAction = new QAction(tr("Show Previous Tab"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
    previousTabAction->setShortcuts(shortcuts);
    connect(previousTabAction, &QAction::triggered, tabWidget, &TabWidget::previousTab);

    QAction *inspectorAction = new QAction(tr("Open inspector in new window"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_I));
    inspectorAction->setShortcuts(shortcuts);
    connect(inspectorAction, &QAction::triggered, [this]() { emit currentTab()->devToolsRequested(currentTab()->page()); });

    connect(menu, &QMenu::aboutToShow, [this, menu, nextTabAction, previousTabAction, inspectorAction]() {
        menu->clear();
        menu->addAction(nextTabAction);
        menu->addAction(previousTabAction);
        menu->addSeparator();
        menu->addAction(inspectorAction);
        menu->addSeparator();

        QList<BrowserWindow*> windows = m_browser->windows();
        int index(-1);
        for (auto window : windows) {
            QAction *action = menu->addAction(window->windowTitle(), this, &BrowserWindow::handleShowWindowTriggered);
            action->setData(++index);
            action->setCheckable(true);
            if (window == this)
                action->setChecked(true);
        }
    });
    return menu;
}

QMenu *BrowserWindow::createHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"));
    helpMenu->addAction(tr("About &Qt"), qApp, QApplication::aboutQt);
    return helpMenu;
}

static bool isBackspace(const QKeySequence &k)
{
    return (k[0].key() & Qt::Key_unknown) == Qt::Key_Backspace;
}

// Chromium already handles navigate on backspace when appropriate.
static QList<QKeySequence> removeBackspace(QList<QKeySequence> keys)
{
    const auto it = std::find_if(keys.begin(), keys.end(), isBackspace);
    if (it != keys.end())
        keys.erase(it);
    return keys;
}

QToolBar *BrowserWindow::createToolBar()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    navigationBar->setMovable(false);
    navigationBar->toggleViewAction()->setEnabled(false);

    // Bouton "Retour"
    m_historyBackAction = new QAction(this);
    auto backShortcuts = removeBackspace(QKeySequence::keyBindings(QKeySequence::Back));
    // For some reason Qt doesn't bind the dedicated Back key to Back.
    backShortcuts.append(QKeySequence(Qt::Key_Back));
    m_historyBackAction->setShortcuts(backShortcuts);
    m_historyBackAction->setIconVisibleInMenu(false);
    m_historyBackAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious,
                                                  QIcon(":go-previous.png"_L1)));
    m_historyBackAction->setToolTip(tr("Go back in history"));
    connect(m_historyBackAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Back);
    });
    navigationBar->addAction(m_historyBackAction);

    // Bouton "Suivant"
    m_historyForwardAction = new QAction(this);
    auto fwdShortcuts = removeBackspace(QKeySequence::keyBindings(QKeySequence::Forward));
    fwdShortcuts.append(QKeySequence(Qt::Key_Forward));
    m_historyForwardAction->setShortcuts(fwdShortcuts);
    m_historyForwardAction->setIconVisibleInMenu(false);
    m_historyForwardAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext,
                                                     QIcon(":go-next.png"_L1)));
    m_historyForwardAction->setToolTip(tr("Go forward in history"));
    connect(m_historyForwardAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Forward);
    });
    navigationBar->addAction(m_historyForwardAction);

    // Bouton "Reload"
    m_stopReloadAction = new QAction(this);
    connect(m_stopReloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::WebAction(m_stopReloadAction->data().toInt()));
    });
    navigationBar->addAction(m_stopReloadAction);

    // Barre URL
    m_urlLineEdit = new QLineEdit(this);
    m_urlLineEdit->setClearButtonEnabled(true);
    navigationBar->addWidget(m_urlLineEdit);

    // Bouton action Favoris
    //m_favAction = new QAction(this);
    //m_urlLineEdit->addAction(m_favAction, QLineEdit::LeadingPosition);

    // Bouton "Ajouter aux favoris"
    m_favAction = new QAction(QIcon(":/icons/favorite.png"), tr("Ajouter aux favoris"), this);
    connect(m_favAction, &QAction::triggered, [this]() {
        QUrl currentUrl = m_tabWidget->currentWebView()->url();
        QString title = m_tabWidget->currentWebView()->title();
        saveFavorite(currentUrl, title);
        QMessageBox::information(this, tr("Favoris"), tr("Ajouté aux favoris :\n%1").arg(title));
    });
    navigationBar->addAction(m_favAction);

    // Bouton "Téléchargements"
    auto downloadsAction = new QAction(this);
    downloadsAction->setIcon(QIcon(u":go-bottom.png"_s));
    downloadsAction->setToolTip(tr("Show downloads"));
    navigationBar->addAction(downloadsAction);
    connect(downloadsAction, &QAction::triggered,
            &m_browser->downloadManagerWidget(), &QWidget::show);

    // Bouton "Paramètres"
    m_settingsMenu = new QMenu(tr("Paramètres"), this);
    m_settingsMenu->addAction(tr("Préférences"), this, []() {
        QMessageBox::information(nullptr, tr("Paramètres"), tr("Ouvrir les préférences"));
    });
    m_settingsMenu->addAction(tr("Extensions"), this, []() {
        QMessageBox::information(nullptr, tr("Extensions"), tr("Gérer les extensions"));
    });
    m_settingsMenu->addAction(tr("À propos"), this, []() {
        QMessageBox::information(nullptr, tr("À propos"), tr("Version 1.0 de WeedlyWeb"));
    });

    m_settingsAction = new QAction(QIcon(":/icons/settings.png"), tr("Paramètres"), this);
    m_settingsAction->setMenu(m_settingsMenu);
    navigationBar->addAction(m_settingsAction);


    return navigationBar;
}

void BrowserWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action) {
    case QWebEnginePage::Back:
        m_historyBackAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Forward:
        m_historyForwardAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
        m_reloadAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Stop:
        m_stopAction->setEnabled(enabled);
        break;
    default:
        qWarning("Unhandled webActionChanged signal");
    }
}

void BrowserWindow::handleWebViewTitleChanged(const QString &title)
{
    QString suffix = m_profile->isOffTheRecord()
        ? tr("Qt Simple Browser (Incognito)")
        : tr("Qt Simple Browser");

    if (title.isEmpty())
        setWindowTitle(suffix);
    else
        setWindowTitle(title + " - " + suffix);
}

void BrowserWindow::handleNewWindowTriggered()
{
    BrowserWindow *window = m_browser->createWindow();
    window->m_urlLineEdit->setFocus();
}

void BrowserWindow::handleNewIncognitoWindowTriggered()
{
    BrowserWindow *window = m_browser->createWindow(/* offTheRecord: */ true);
    window->m_urlLineEdit->setFocus();
}

void BrowserWindow::handleFileOpenTriggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QString(),
                                                tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (url.isEmpty())
        return;
    currentTab()->setUrl(url);
}

void BrowserWindow::handleFindActionTriggered()
{
    if (!currentTab())
        return;
    bool ok = false;
    QString search = QInputDialog::getText(this, tr("Find"),
                                           tr("Find:"), QLineEdit::Normal,
                                           m_lastSearch, &ok);
    if (ok && !search.isEmpty()) {
        m_lastSearch = search;
        currentTab()->findText(m_lastSearch);
    }
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("Are you sure you want to close the window ?\n"
                                          "There are %1 tabs open.").arg(m_tabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

TabWidget *BrowserWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

void BrowserWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon = QIcon::fromTheme(QIcon::ThemeIcon::ProcessStop,
                                             QIcon(":process-stop.png"_L1));
    static QIcon reloadIcon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh,
                                               QIcon(":view-refresh.png"_L1));

    if (0 < progress && progress < 100) {
        m_stopReloadAction->setData(QWebEnginePage::Stop);
        m_stopReloadAction->setIcon(stopIcon);
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
        m_progressBar->setValue(progress);
    } else {
        m_stopReloadAction->setData(QWebEnginePage::Reload);
        m_stopReloadAction->setIcon(reloadIcon);
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
        m_progressBar->setValue(0);
    }
}

void BrowserWindow::handleShowWindowTriggered()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        int offset = action->data().toInt();
        QList<BrowserWindow*> windows = m_browser->windows();
        windows.at(offset)->activateWindow();
        windows.at(offset)->currentTab()->setFocus();
    }
}

void BrowserWindow::handleDevToolsRequested(QWebEnginePage *source)
{
    source->setDevToolsPage(m_browser->createDevToolsWindow()->currentTab()->page());
    source->triggerAction(QWebEnginePage::InspectElement);
}

void BrowserWindow::handleFindTextFinished(const QWebEngineFindTextResult &result)
{
    if (result.numberOfMatches() == 0) {
        statusBar()->showMessage(tr("\"%1\" not found.").arg(m_lastSearch));
    } else {
        statusBar()->showMessage(tr("\"%1\" found: %2/%3").arg(m_lastSearch,
                                                               QString::number(result.activeMatch()),
                                                               QString::number(result.numberOfMatches())));
    }
}


void BrowserWindow::saveFavorite(const QUrl &url, const QString &title)
{
    QFile file("favorites.json");

    QJsonArray favoritesArray;
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
        favoritesArray = loadDoc.array();
        file.close();
    }

    // Vérificiation pour éviter les doublons
    for (const QJsonValue &value : favoritesArray) {
        if (value.toObject().value("url").toString() == url.toString()) {
            QMessageBox::information(this, tr("Favoris"), tr("Ce favori existe déjà."));
            return;
        }
    }

    // Ajouter le nouvel élément
    QJsonObject newFavorite;
    newFavorite["title"] = title;
    newFavorite["url"] = url.toString();
    favoritesArray.append(newFavorite);

    // Sauvegarde dans le fichier JSON
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(favoritesArray);
        file.write(saveDoc.toJson());
        file.close();
    }
}


void BrowserWindow::loadFavorites()
{
    QFile file("favorites.json");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray favoritesArray = doc.array();

    for (const QJsonValue &value : favoritesArray) {
        QJsonObject obj = value.toObject();
        qDebug() << "Favori chargé:" << obj["title"].toString() << obj["url"].toString();
    }
    file.close();
}


void BrowserWindow::setupFavoritesBar() {
    m_favoritesBar->clear();
    m_favoritesBar->setMovable(false);

    // Ajouter un bouton "Voir plus" pour les favoris excédentaires
    m_moreFavoritesAction = new QAction(QIcon(":/icons/more.png"), tr("Voir plus..."), this);
    connect(m_moreFavoritesAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(this, tr("Favoris"), tr("Afficher la liste complète des favoris."));
    });

    m_favoritesBar->addAction(m_moreFavoritesAction);
}


void BrowserWindow::loadFavoritesToBar()
{
    QFile file("favorites.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray favoritesArray = doc.array();
    file.close();

    m_favoritesBar->clear();
    int maxVisibleFavorites = 5;  // Maximum de favoris affichés dans la barre
    int count = 0;

    for (const QJsonValue &value : favoritesArray) {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        QUrl url = QUrl(obj["url"].toString());

        QAction *favAction = new QAction(QIcon(":/icons/star.png"), title, this);
        connect(favAction, &QAction::triggered, this, [this, url]() {
            openFavorite(url);
        });

        m_favoritesBar->addAction(favAction);
        count++;

        if (count >= maxVisibleFavorites) {
            break;
        }
    }

    if (favoritesArray.size() > maxVisibleFavorites) {
        m_favoritesBar->addAction(m_moreFavoritesAction);
    }
}

void BrowserWindow::openFavorite(const QUrl &url)
{
    m_tabWidget->setUrl(url);
    m_urlLineEdit->setText(url.toString());
}
