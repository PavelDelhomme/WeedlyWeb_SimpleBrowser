// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "browser.h"
#include "browserwindow.h"
#include "downloadmanagerwidget.h"
#include "tabwidget.h"
#include "webview.h"
#include "commandwidget.h"
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
#include <QComboBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QListWidget>
#include <QTextEdit>
#include <QShortcut>
#include <QTableWidget>
#include <QHeaderView>
#include <QPainter>
#include <QCompleter>
#include <QFile>
#include <QStringListModel>

using namespace Qt::StringLiterals;

BrowserWindow::BrowserWindow(Browser *browser, QWebEngineProfile *profile, bool forDevTools)
    : m_browser(browser)
    , m_profile(profile)
    , m_tabWidget(new TabWidget(profile, this))
    , m_progressBar(new QProgressBar(this))
    , m_historyBackAction(nullptr)
    , m_stopAction(nullptr)
    , m_reloadAction(nullptr)
    , m_stopReloadAction(nullptr)
    , m_urlLineEdit(nullptr)
    , m_favAction(new QAction(this))
    , m_settingsMenu(nullptr)
    , m_settingsAction(nullptr)
    , m_toolbar(createToolBar())
    , m_favoritesBar(new QToolBar(tr("Favoris"), this))  // Initialisation de la barre de favoris
    , m_moreFavoritesAction(nullptr)
    , m_favoritesMenu(nullptr)
    , m_urlCompleter(new QCompleter(this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);


    if (!forDevTools) {
        addToolBar(m_toolbar);

        // Barre de favoris ajoutée avant les onglets
        m_favoritesBar->setMovable(false);

        setupFavoritesBar();
        setupFavoritesMenu();
        loadFavoritesToBar();
        updateUrlCompleter();

        menuBar()->addMenu(createFileMenu(m_tabWidget));
        menuBar()->addMenu(createEditMenu());
        menuBar()->addMenu(createViewMenu(m_toolbar));
        menuBar()->addMenu(createWindowMenu(m_tabWidget));
        menuBar()->addMenu(createHelpMenu());
        menuBar()->addMenu(createCommandMenu());
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    if (!forDevTools) {
        addToolBarBreak();

        m_progressBar = new QProgressBar(this);

        if (m_progressBar) {
            m_progressBar->setMaximumHeight(1);
            m_progressBar->setTextVisible(false);
            m_progressBar->setStyleSheet(u"QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"_s);
            layout->addWidget(m_progressBar);
        }
    }

    layout->addWidget(m_toolbar);

    layout->addWidget(m_favoritesBar);

    m_favAction = new QAction(this);
    if (m_favAction) {
        m_favAction->setIcon(QIcon(":/star-regular.png"));
    } else {
        qDebug() << "m_favAction is null in BrowserWindow constructor";
    }
    if (m_favAction) {
        m_favAction->setToolTip(tr("Ajouter/Supprimer des favoris"));
    } else {
        qDebug() << "m_favAction is null when setting tooltip";
    }

    connect(m_favAction, &QAction::triggered, this, &BrowserWindow::handleFavActionTriggered);


    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    if (m_tabWidget) {
        connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
        connect(m_tabWidget, &TabWidget::currentChanged, this, [this](int index) {
            if (WebView *view = m_tabWidget->currentWebView()) {
                updateFavoriteIcon(view->url()); // Mise à jour immédiate de l'icône au changement d'onglet
                connect(view, &WebView::loadFinished, this, [this](bool ok) {
                    updateFavoriteIcon(currentTab()->url(), ok);
                });
            }
        });
    }
    connect(m_tabWidget, &TabWidget::urlChanged, this, [this](const QUrl &url) {
        updateFavoriteIcon(url);
    });
    // connect(m_tabWidget, &TabWidget::favIconChanged, this, &BrowserWindow::updateFavoriteIcon);
    connect(m_tabWidget, &TabWidget::favIconChanged, this, [this](const QIcon &icon) {
        WebView *view = currentTab();
        if (view) {
            updateFavoriteIcon(view->url());
        }
    });

    if (!forDevTools) {
        if (m_tabWidget) {
            connect(m_tabWidget, &TabWidget::linkHovered, [this](const QString& url) {
                statusBar()->showMessage(url);
            });
            connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
        }
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

    // Commandes
    m_commandWidget = new CommandWidget(this);
    m_commandWidget->setGeometry(50, 50, 300, 30);
    m_commandWidget->hide();
    layout->addWidget(m_commandWidget);

    // Pour ouvrir la commande faire CTRL + ALT + C
    QShortcut *commandShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_C), this);
    connect(commandShortcut, &QShortcut::activated, this, &BrowserWindow::toggleCommandWidget);


    // Request interceptor
    m_requestInterceptor = new RequestInterceptor(this);
    profile->setUrlRequestInterceptor(m_requestInterceptor);

    connect(m_commandWidget, &CommandWidget::commandEntered, this, &BrowserWindow::processCommand);

    m_urlCompleter = new QCompleter(this);
    m_urlCompleter->setFilterMode(Qt::MatchContains);
    m_urlCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_urlLineEdit->setCompleter(m_urlCompleter);

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

void BrowserWindow::toggleCommandWidget()
{
    if (m_commandWidget->isVisible()) {
        m_commandWidget->hide();
        m_commandWidget->clear();
    } else {
        m_commandWidget->show();
        m_commandWidget->setFocus();
    }
}

QMenu *BrowserWindow::createCommandMenu()
{
    QMenu *commandMenu = new QMenu(tr("&Commandes"));
    QAction *showCommandBarAction = commandMenu->addAction(tr("Afficher/Masquer la barre de commande"));
    showCommandBarAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_C));
    showCommandBarAction->setShortcutVisibleInContextMenu(true);

    connect(showCommandBarAction, &QAction::triggered, this, &BrowserWindow::toggleCommandWidget);

    return commandMenu;
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

    // Bouton "Téléchargements"
    auto downloadsAction = new QAction(this);
    downloadsAction->setIcon(QIcon(u":go-bottom.png"_s));
    downloadsAction->setToolTip(tr("Show downloads"));
    navigationBar->addAction(downloadsAction);
    connect(downloadsAction, &QAction::triggered,
            &m_browser->downloadManagerWidget(), &QWidget::show);

    // Bouton "Favoris'
    auto m_favAction = new QAction(this);
    m_favAction->setIcon(QIcon(":/star-regular.png"));
    m_favAction->setToolTip(tr("Ajouter/Supprimer des favoris"));
    navigationBar->addAction(m_favAction);
    connect(m_favAction, &QAction::triggered, this, &BrowserWindow::handleFavActionTriggered);

    // Bouton "Paramètres"
    m_settingsMenu = new QMenu(tr("Paramètres"), this);
    m_settingsMenu->addSeparator();
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
    return m_tabWidget ? m_tabWidget->currentWebView() : nullptr;
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
    loadFavoritesToBar();
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
    // connect(m_moreFavoritesAction, &QAction::triggered, this, [this]() {
        // QMessageBox::information(this, tr("Favoris"), tr("Afficher la liste complète des favoris."));
    // });
    connect(m_moreFavoritesAction, &QAction::triggered, this, &BrowserWindow::showFavoritesManager);

    m_favoritesBar->addAction(m_moreFavoritesAction);
}

void BrowserWindow::loadFavoritesToBar()
{
    /*
    QFile file("favorites.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray favoritesArray = doc.array();
    file.close();

    m_favoritesBar->clear();

    for (const QJsonValue &value : favoritesArray) {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        QUrl url = QUrl(obj["url"].toString());
        QString faviconPath = obj["favicon"].toString();

        QAction *favAction = new QAction(title, this);
        if (!faviconPath.isEmpty() && QFile::exists(faviconPath)) {
            favAction->setIcon(QIcon(faviconPath));
        } else {
            // Utiliser la première lettre du titre comme icône par défaut
            QPixmap pixmap(16, 16);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.drawText(pixmap.rect(), Qt::AlignCenter, title.left(1).toUpper());
            favAction->setIcon(QIcon(pixmap));
        }

        favAction->setText(title); // S'assurrer que le titre est toujours définis
        //favAction->setToolTip(url.toString()); // Ajouter l'url comme infobulle

        connect(favAction, &QAction::triggered, this, [this, url]() {
            openFavorite(url);
        });

        // Créer un menu contextuel pour l'action
        QMenu *contextMenu = new QMenu(this);
        QAction *editAction = contextMenu->addAction(tr("Modifier le favori"));
        QAction *deleteAction = contextMenu->addAction(tr("Supprimer le favori"));

        connect(editAction, &QAction::triggered, this, [this, title, url] (){
            editFavorite(title, url);
        });
        connect(deleteAction, &QAction::triggered, this, [this, url]() {
            deleteFavorite(url);
        });

        favAction->setMenu(contextMenu);

        m_favoritesBar->addAction(favAction);
    }
    */
    m_favoritesBar->clear();
    for (auto* item : m_favoritesRoot->children) {
        addFavoriteToBar(item, m_favoritesBar);
    }
}


void BrowserWindow::addFavoriteToBar(FavoriteItem* item, QWidget* parent)
{
    if (item->url.isEmpty()) {
        // Gestion des dossiers
        QMenu* folderMenu = new QMenu(item->title, parent);
        folderMenu->setIcon(QIcon(":/folder-closed.png"));

        for (FavoriteItem* child : item->children) {
            addFavoriteToBar(child, folderMenu);
        }

        if (QToolBar* tb = qobject_cast<QToolBar*>(parent)) {
            tb->addAction(folderMenu->menuAction());
        }
    } else {
        // Gestion des favoris
        QAction* action = new QAction(parent);
        action->setText(item->title);
        action->setToolTip(item->url);

        //if (!item->iconPath.isEmpty() && QFile::exists(item->iconPath)) {
        if (!item->iconPath.isEmpty()) {
            action->setIcon(QIcon(item->iconPath));
        } else {
            // Générer une icône à partir de la première lettre
            QPixmap pix(16, 16);
            pix.fill(Qt::transparent);
            QPainter painter(&pix);
            //painter.setPen(Qt::black);
            //painter.setFont(QFont("Arial", 12, QFont::Bold));
            //painter.drawText(pixmap.rect(), Qt::AlignCenter, item->title.left(1).toUpper());
            painter.drawText(pix.rect(), Qt::AlignCenter, item->title.left(1));
            action->setIcon(QIcon(pix));
        }

        //connect(action, &QAction::triggered, this, [this, item]() {
        //     openFavorite(QUrl(item->url));
        // });
        // if (parent == m_favoritesBar) {
        //     m_favoritesBar->addAction(action);
        // } else {
        //     static_cast<QMenu*>(parent)->addAction(action);
        // }
        connect(action, &QAction::triggered, this, [this, item]() {
            m_tabWidget->setUrl(QUrl(item->url));
        });

        if (QToolBar* tb = qobject_cast<QToolBar*>(parent)) {
            tb->addAction(action);
        }
    }
}

void BrowserWindow::loadFavoritesToBarRecursive(const QJsonArray& array, QWidget* parent)
{
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString name = obj["name"].toString();
        QString url = obj["url"].toString();

        if (obj.contains("children")) {
            QMenu* subMenu = new QMenu(name, parent);
            loadFavoritesToBarRecursive(obj["children"].toArray(), subMenu);
            if (parent == m_favoritesBar) {
                m_favoritesBar->addAction(subMenu->menuAction());
            } else {
                static_cast<QMenu*>(parent)->addMenu(subMenu);
            }
        } else {
            QAction* action = new QAction(name, parent);
            connect(action, &QAction::triggered, this, [this, url]() {
                openFavorite(QUrl(url));
            });
            if (parent == m_favoritesBar) {
                m_favoritesBar->addAction(action);
            } else {
                static_cast<QMenu*>(parent)->addAction(action);
            }
        }
    }
}

void BrowserWindow::showFavoriteContextMenu(const QPoint &pos, const QString &title, const QUrl &url)
{
    QMenu contextMenu(this);
    QAction *editAction = contextMenu.addAction(tr("Modifier le favori"));
    QAction *deleteAction = contextMenu.addAction(tr("Supprimer le favori"));

    QAction *selectedAction = contextMenu.exec(m_favoritesBar->mapToGlobal(pos));
    if (selectedAction == editAction) {
        editFavorite(title, url);
    } else if (selectedAction == deleteAction) {
        deleteFavorite(url);
    }
}



void BrowserWindow::editFavorite(const QString &oldTitle, const QUrl &oldUrl)
{
    QDialog dialog(this);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *titleEdit = new QLineEdit(oldTitle, &dialog);
    QLineEdit *urlEdit = new QLineEdit(oldUrl.toString(), &dialog);
    QComboBox *folderCombo = new QComboBox(&dialog);
    // Remplir folderCombo avec les dossiers de favoris existants

    layout->addWidget(new QLabel(tr("Titre:"), &dialog));
    layout->addWidget(titleEdit);
    layout->addWidget(new QLabel(tr("URL:"), &dialog));
    layout->addWidget(urlEdit);
    layout->addWidget(new QLabel(tr("Dossier:"), &dialog));
    layout->addWidget(folderCombo);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        updateFavorite(oldUrl, titleEdit->text(), QUrl(urlEdit->text()), folderCombo->currentText());
    }
}


void BrowserWindow::openFavorite(const QUrl &url)
{
    m_tabWidget->setUrl(url);
    m_urlLineEdit->setText(url.toString());
}

void BrowserWindow::setupFavoritesMenu()
{
    m_favoritesMenu = new QMenu(tr("Favoris"), this);
    m_toolbar->addAction(m_favoritesMenu->menuAction());

    QAction *addFavoriteAction = m_favoritesMenu->addAction(tr("Ajouter aux favoris"));

    connect(addFavoriteAction, &QAction::triggered, this, &BrowserWindow::addCurrentPageToFavorites);

    QAction *manageFavoritesAction = m_favoritesMenu->addAction(tr("Gérer les favoris"));
    connect(manageFavoritesAction, &QAction::triggered, this, &BrowserWindow::showFavoritesManager);

    m_favoritesMenu->addSeparator();

    loadFavorites();
}


void BrowserWindow::addCurrentPageToFavorites()
{
    WebView *webView = currentTab();
    if (webView) {
        QString title = webView->title();
        QString url = webView->url().toString();

        bool ok;
        QString name = QInputDialog::getText(this, tr("Ajouter aux favoris"), tr("Nom du favori:"), QLineEdit::Normal, title, &ok);
        if (ok && !name.isEmpty()) {
            addFavorite(name, url);
        }
    }
}

void BrowserWindow::addFavorite(const QString &name, const QString &url)
{
    // Ajouter le favori à la struture de données
    m_favorites.append({name, url});

    // Ajouter le favori au menu
    QAction *favoriteAction = m_favoritesMenu->addAction(name);
    connect(favoriteAction, &QAction::triggered, this, [this, url]() {
        m_tabWidget->setUrl(QUrl(url));
    });

    // Sauvegarder les favoris
    saveFavorite(QUrl(url), name);
    // Mise a jour du completer
    updateUrlCompleter();
}

void BrowserWindow::handleFavActionTriggered()
{
    WebView *webView = currentTab();
    if (webView) {
        QUrl currentUrl = webView->url();
        QString currentTitle = webView->title();

        if (isFavorite(currentUrl)) {
            int ret = QMessageBox::question(this, tr("Supprimer le favori"),
                                            tr("Voulez-vous vraiment supprimer ce favori ?"),
                                            QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes) {
                deleteFavorite(currentUrl);
                m_favAction->setIcon(QIcon(":/star-regular.png"));
            }
        } else {
            bool ok;
            QString name = QInputDialog::getText(this, tr("Ajouter aux favoris"),
                                                 tr("Nom du favori:"), QLineEdit::Normal,
                                                 currentTitle, &ok);
            if (ok && !name.isEmpty()) {
                addFavorite(name, currentUrl.toString());
                m_favAction->setIcon(QIcon(":/star-solid.png"));
            }
        }
        updateFavoriteIcon(currentUrl);
        loadFavoritesToBar();
    }
    // if (webView) {
    //     QUrl currentUrl = webView->url();
    //     QString currentTitle = webView->title();

    //     bool favoriteExists = isFavorite(currentUrl);

    //     if (favoriteExists) {
    //         deleteFavorite(currentUrl);
    //         updateFavoriteIcon();
    //         QMessageBox::information(this, tr("Favoris"), tr("Favori supprimé"));
    //     } else {
    //         bool ok;
    //         QString name = QInputDialog::getText(this, tr("Ajouter aux favoris"), tr("Nom du favori:"), QLineEdit::Normal, currentTitle, &ok);
    //         if (ok && !name.isEmpty()) {
    //             addFavorite(name, currentUrl.toString());
    //             updateFavoriteIcon();
    //             QMessageBox::information(this, tr("Favoris"), tr("Ajouté aux favoris :\n%1").arg(name));
    //         }
    //     }
    // }
}

bool BrowserWindow::isFavorite(const QUrl &url) const
{
    qDebug() << "Vérification si l'URL est un favori:" << url.toString();
    QFile file("favorites.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        file.close();

        qDebug() << "Nombre de favoris trouvés:" << favoritesArray.size();

        for (const QJsonValue &value : favoritesArray) {
            if (value.toObject()["url"].toString() == url.toString()) {
                qDebug() << "Favori trouvé pour l'URL:" << url.toString();
                return true;
            }
        }
    } else {
        qWarning() << "Impossible d'ouvrir le fichier favorites.json";
    }
    qDebug() << "URL non trouvée dans les favoris";
    return false;
}


void BrowserWindow::deleteFavorite(const QUrl &url)
{
    QFile file("favorites.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        file.close();

        for (int i = 0; i < favoritesArray.size(); ++i) {
            if (favoritesArray[i].toObject()["url"].toString() == url.toString()) {
                favoritesArray.removeAt(i);
                break;
            }
        }

        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument saveDoc(favoritesArray);
            file.write(saveDoc.toJson());
            file.close();
        }
    }
    loadFavoritesToBar();
    updateUrlCompleter();
}

void BrowserWindow::updateFavorite(const QUrl &oldUrl, const QString &newTitle, const QUrl &newUrl, const QString &newFolder)
{
    QFile file("favorites.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        file.close();

        for (int i = 0; i < favoritesArray.size(); ++i) {
            QJsonObject favorite = favoritesArray[i].toObject();
            if (favorite["url"].toString() == oldUrl.toString()) {
                favorite["title"] = newTitle;
                favorite["url"] = newUrl.toString();
                favorite["folder"] = newFolder;
                favoritesArray[i] = favorite;
                break;
            }
        }

        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument saveDoc(favoritesArray);
            file.write(saveDoc.toJson());
            file.close();
        }
    }
    loadFavoritesToBar();
}


void BrowserWindow::handleWebViewLoadFinished(bool ok)
{
    if (ok) {
        WebView *view = qobject_cast<WebView*>(sender());
        if (view) {
            QUrl url = view->url();
            updateFavoriteIcon(url);
            QString faviconUrl = url.scheme() + "://" + url.host() + "/favicon.ico";

            // Télécharger la favicon
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
            connect(manager, &QNetworkAccessManager::finished, this, [this, url, manager](QNetworkReply *reply) {
                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray data = reply->readAll();
                    QString faviconPath = saveFavicon(data, url);
                    updateFaviconForFavorite(url, faviconPath);
                    QIcon icon(faviconPath);
                    updateFaviconInFavoritesBar(url, icon);
                }
                reply->deleteLater();
                manager->deleteLater();
            });
            manager->get(QNetworkRequest(QUrl(faviconUrl)));
        }
    }
}


QString BrowserWindow::saveFavicon(const QByteArray &data, const QUrl &url)
{
    QString fileName = url.host() + ".ico";
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/favicons/" + fileName;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    }
    return filePath;
}

void BrowserWindow::updateFaviconForFavorite(const QUrl &url, const QString &faviconPath)
{
    QFile file("favorites.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        file.close();

        for (int i = 0; i < favoritesArray.size(); ++i) {
            QJsonObject favorite = favoritesArray[i].toObject();
            if (favorite["url"].toString() == url.toString()) {
                favorite["favicon"] = faviconPath;
                favoritesArray[i] = favorite;
                break;
            }
        }

        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument saveDoc(favoritesArray);
            file.write(saveDoc.toJson());
            file.close();
        }
    }

    QIcon icon(faviconPath);
    updateFaviconInFavoritesBar(url, icon);

    loadFavoritesToBar();
}


void BrowserWindow::showFavoritesManager()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Ajouter aux favoris"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QTreeWidget *tree = new QTreeWidget(&dialog);
    tree->setHeaderLabel({tr("Dossiers")});
    populateFolderTree(tree->invisibleRootItem(), m_favoritesRoot);

    QLineEdit *nameEdit = new QLineEdit(&dialog);
    QLineEdit *urlEdit = new QLineEdit(currentTab()->url().toString(), &dialog);

    layout->addWidget(new QLabel(tr("Nom:")));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel(tr("URL:")));
    layout->addWidget(urlEdit);
    layout->addWidget(new QLabel(tr("Dossier parent:")));
    layout->addWidget(tree);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        FavoriteItem* parentFolder = getSelectedFolder(tree);
        addFavoriteToFolder(nameEdit->text(), urlEdit->text(), parentFolder);
    }
}
void BrowserWindow::addFavoriteToFolder(const QString &name, const QString &url, FavoriteItem* folder)
{
    FavoriteItem* newFavorite = new FavoriteItem{
        name,
        url,
        "", // iconPath, vous pouvez le laisser vide pour l'instant
        {},
        folder
    };

    folder->children.append(newFavorite);

    // Mettre à jour l'interface utilisateur si nécessaire
    loadFavoritesToBar();

    // Sauvegarder les favoris
    saveFavoritesToJson();
}


void BrowserWindow::populateFolderTree(QTreeWidgetItem* parentItem, FavoriteItem* folder)
{
    for (FavoriteItem* item : folder->children) {
        if (item->url.isEmpty()) { // C'est un dossier
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(parentItem, {item->title});
            populateFolderTree(treeItem, item);
        }
    }
}

void BrowserWindow::saveFavoritesFromTree(QTreeWidget *tree)
{
    QFile file("favorites.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray favoritesArray;

        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            auto item = tree->topLevelItem(i);
            QJsonObject obj;
            obj["title"] = item->text(0);
            obj["url"] = item->text(1);
            obj["folder"] = item->text(2); // Si vous avez des dossiers
            favoritesArray.append(obj);
        }

        file.write(QJsonDocument(favoritesArray).toJson());
        file.close();
    }
}


void BrowserWindow::updateFavoriteIcon(const QUrl &url, bool ok)
{
    qDebug() << "Vérification de l'URL pour les favoris:" << url.toString() << "Chargement réussi:" << ok;

    if (!m_favAction) {
        qWarning() << "m_favAction non initialisé !";
        return;
    }

    //WebView *webView = currentTab();
    // if (!webView) {
    //     m_favAction->setIcon(QIcon(":/star-regular.png"));
    //     return;
    // }

    QUrl currentUrl = url;
    bool isFav = isFavorite(currentUrl);

    // Mettre a jour l'icône et le tooltip
    m_favAction->setIcon(QIcon(isFav ? ":/star-solid.png" : ":/star-regular.png"));
    m_favAction->setToolTip(isFav ? tr("Supprimer des favoris") : tr("Ajouter aux favoris"));
    WebView *view = currentTab();
    if (view) {
        QUrl url = view->url();
        if (isFavorite(url)) {
            // Mettre à jour l'icône dans la barre de favoris
            for (QAction *action : m_favoritesBar->actions()) {
                if (action->data().toUrl() == url) {
                    // action->setIcon(icon);
                    break;
                }
            }
        }
    }
    if (!ok) {
        // Gérer le cas où le chargement à échoué
        qDebug() << "Le chargement de la page à échoué";
    }
}



void BrowserWindow::processCommand(const QString &command)
{
    if (command.startsWith("/cvec")) {
        processCVECommand(command);
    } else if (command.startsWith("/request")) {
        processRequestCommand(command);
    } else if (command.startsWith("/analyze")) {
        showRequestAnalyzer();
    } else {
        // Gérer les commandes inconnues
        qDebug() << "Commande inconnue : " << command;
    }
}

void BrowserWindow::processCVECommand(const QString &command)
{
    if (command == "/cvec detect") {
        WebView *currentView = currentTab();
        if (currentView) {
            currentView->page()->toHtml([this](const QString &result) {
                QStringList detectedCVEs = detectCVEs(result);
                displayCVEResults(detectedCVEs);
            });
        }
    }
}


void BrowserWindow::processRequestCommand(const QString &command)
{
    QStringList parts = command.split(" ");
    if (parts.size() >= 2) {
        QString method = parts[1];
        QString url = currentTab()->url().toString();

        if (method == "GET") {
            sendGetRequest(url);
        } else if (method == "POST") {
            sendPostRequest(url);
        } else if (method == "analyze") {
            showRequestAnalyzer();
        }
    }
}

QStringList BrowserWindow::detectCVEs(const QString &html) {
    // Implementer la logique de détection des CVE
    // Exemple simplifié
    QStringList cves;
    QRegularExpression re("CVE-\\d{4}-\\d{4,7}");
    QRegularExpressionMatchIterator i = re.globalMatch(html);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        cves << match.captured(0);
    }
    return cves;
}

void BrowserWindow::displayCVEResults(const QStringList &cves)
{
    QDialog *resultDialog = new QDialog(this);
    resultDialog->setWindowTitle(tr("Résultats de détection CVE"));
    QVBoxLayout *layout = new QVBoxLayout(resultDialog);

    QListWidget *listWidget = new QListWidget(resultDialog);
    listWidget->addItems(cves);
    layout->addWidget(listWidget);

    resultDialog->setLayout(layout);
    resultDialog->show();
}


void BrowserWindow::sendPostRequest(const QString &url) {
    // Implementer l'envoi de requête POST
    // Vous devrez probablement demander les données du corps de la requête à l'utilisateur
}


void BrowserWindow::handleNetworkReply(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Erreur : " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    QDialog *resultDialog = new QDialog(this);
    resultDialog->setWindowTitle("Résultat de la requête");
    QVBoxLayout *layout = new QVBoxLayout(resultDialog);

    QTextEdit *textEdit = new QTextEdit(resultDialog);
    textEdit->setPlainText(answer);
    layout->addWidget(textEdit);

    resultDialog->setLayout(layout);
    resultDialog->show();

    reply->deleteLater();
}

void BrowserWindow::sendGetRequest(const QString &url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &BrowserWindow::handleNetworkReply);
    manager->get(QNetworkRequest(QUrl(url)));
}

void BrowserWindow::showRequestAnalyzer()
{
    QDialog *analyzerDialog = new QDialog(this);
    analyzerDialog->setWindowTitle(tr("Analyseur de requêtes"));
    QVBoxLayout *layout = new QVBoxLayout(analyzerDialog);

    QTableWidget *requestTable = new QTableWidget(analyzerDialog);
    requestTable->setColumnCount(3);
    requestTable->setHorizontalHeaderLabels({tr("ID"), tr("URL"), tr("Méthode")});
    requestTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Assurez-vous que m_requestInterceptor est correctement initialisé
    if (m_requestInterceptor) {
        auto requests = m_requestInterceptor->getInterceptedRequests();
        requestTable->setRowCount(requests.size());

        int row = 0;
        for (auto it = requests.begin(); it != requests.end(); ++it, ++row) {
            requestTable->setItem(row, 0, new QTableWidgetItem(it.key()));
            requestTable->setItem(row, 1, new QTableWidgetItem(it.value().url().toString()));
            requestTable->setItem(row, 2, new QTableWidgetItem(it.value().attribute(QNetworkRequest::CustomVerbAttribute).toString()));
        }
    }

    layout->addWidget(requestTable);

    QPushButton *closeButton = new QPushButton(tr("Fermer"), analyzerDialog);
    connect(closeButton, &QPushButton::clicked, analyzerDialog, &QDialog::accept);
    layout->addWidget(closeButton);

    analyzerDialog->setLayout(layout);
    analyzerDialog->exec();
}


void BrowserWindow::updateUrlCompleter()
{
    QStringList urls;
    QFile file("favorites.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        file.close();

        for (const QJsonValue &value : favoritesArray) {
            QJsonObject obj = value.toObject();
            urls << obj["url"].toString();
        }
    }
    m_urlCompleter->setModel(new QStringListModel(urls, m_urlCompleter));
}


void BrowserWindow::loadFavoritesFromJson()
{
    QFile file("favorites.json");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    buildFavoriteTree(doc.array(), m_favoritesRoot);
    file.close();
}

void BrowserWindow::buildFavoriteTree(const QJsonArray& array, FavoriteItem* parent)
{
    for (const QJsonValue& value : array) {
        QJsonObject obj = value.toObject();
        FavoriteItem* item = new FavoriteItem{
            obj["title"].toString(),
            obj["url"].toString(),
            obj["iconPath"].toString(),
            {},
            parent
        };

        if (obj.contains("children")) {
            buildFavoriteTree(obj["children"].toArray(), item);
        }
        parent->children.append(item);
    }
}


void BrowserWindow::saveFavoritesToJson()
{
    QJsonArray favoritesArray;
    serializeFavoriteTree(m_favoritesRoot, favoritesArray);

    QFile file("favorites.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(favoritesArray);
        file.write(saveDoc.toJson());
        file.close();
    }
}


void BrowserWindow::serializeFavoriteTree(FavoriteItem* root, QJsonArray& array)
{
    for (FavoriteItem* item : root->children) {
        QJsonObject obj;
        obj["title"] = item->title;
        obj["url"] = item->url;
        obj["iconPath"] = item->iconPath;

        if (!item->children.isEmpty()) {
            QJsonArray childrenArray;
            serializeFavoriteTree(item, childrenArray);
            obj["children"] = childrenArray;
        }
        array.append(obj);
    }
}


FavoriteItem* BrowserWindow::getSelectedFolder(QTreeWidget* tree)
{
    QTreeWidgetItem* selectedItem = tree->currentItem();
    if (!selectedItem) {
        return m_favoritesRoot; // Retourne le dossier racine si rien n'est sélectionné
    }

    // Parcourir l'arborescence pour trouver le FavoriteItem correspondant
    QTreeWidgetItem* currentItem = selectedItem;
    QList<QString> path;
    while (currentItem) {
        path.prepend(currentItem->text(0));
        currentItem = currentItem->parent();
    }

    FavoriteItem* current = m_favoritesRoot;
    for (const QString& name : path) {
        bool found = false;
        for (FavoriteItem* child : current->children) {
            if (child->title == name && child->url.isEmpty()) { // C'est un dossier
                current = child;
                found = true;
                break;
            }
        }
        if (!found) {
            return m_favoritesRoot; // Si le chemin n'existe pas, retourne le dossier racine
        }
    }

    return current;
}


void BrowserWindow::updateFaviconInFavoritesBar(const QUrl &url, const QIcon &icon)
{
    for (QAction *action : m_favoritesBar->actions()) {
        if (action->data().toUrl() == url) {
            action->setIcon(icon);
            break;
        }
    }
}
