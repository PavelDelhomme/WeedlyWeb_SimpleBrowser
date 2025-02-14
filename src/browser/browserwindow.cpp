#include "browser.h"
#include "browserwindow.h"
#include "../downloads/downloadmanagerwidget.h"
#include "favoritesmanager.h"
#include "tabwidget.h"
#include "webview.h"
// #include "commandwidget.h"
#include "commandpalette.h"
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
#include <QDrag>
#include <QMimeData>
#include <QFormLayout>
#include <QBuffer>
#include <QTimer>

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
    , m_favoritesRoot(new FavoriteItem(-1, "Root", "", "", {}, nullptr)) // Initialisation de m_favoritesRoot
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    // Initialiser la base de données
    if (!m_database.initDatabase()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'initialiser la base de données"));
        return;
    }

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
        // Définir l'icône initiale
        QColor initialColor = Qt::white; // Couleur initiale
        QPixmap pix(16, 16);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setPen(QPen(initialColor));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(pix.rect(), Qt::AlignCenter, "F");
        m_favAction->setIcon(QIcon(pix));
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

    
    // Créer l'instance de CommandPalette
    m_commandPalette = new CommandPalette(this);
    m_commandPalette->hide(); // Cacher la palette au démarrage
    connect(m_commandPalette, &CommandPalette::commandSelected, this, &BrowserWindow::onCommandPaletteCommandSelected);

    // Request interceptor
    m_requestInterceptor = new RequestInterceptor(this);
    profile->setUrlRequestInterceptor(m_requestInterceptor);

    m_commandPalette->setCurrentWebView(currentTab());
    m_commandPalette->setRequestInterceptor(m_requestInterceptor);

    // Pour ouvrir la commande faire CTRL + ALT + C
    //QShortcut *commandShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), this);
    //connect(commandShortcut, &QShortcut::activated, this, &BrowserWindow::showCommandPalette);
    QShortcut *commandShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Slash), this);
    connect(commandShortcut, &QShortcut::activated, this, &BrowserWindow::toggleCommandWidget);

    // Duplication de tab
    QShortcut *duplicateTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this);
    connect(duplicateTabShortcut, &QShortcut::activated, this, &BrowserWindow::duplicateCurrentTab);

    m_urlCompleter = new QCompleter(this);
    m_urlCompleter->setFilterMode(Qt::MatchContains);
    m_urlCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_urlLineEdit->setCompleter(m_urlCompleter);

    m_favoritesManager = new FavoritesManager(this);
    m_favoritesRoot = new FavoriteItem(-1, "Root", "", "", {}, nullptr);
    
    loadFavoritesFromDatabase();
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
    if (m_commandPalette->isVisible()) {
        m_commandPalette->hide();
    } else {
        showCommandPalette();
    }
}

QMenu *BrowserWindow::createCommandMenu()
{
    QMenu *commandMenu = new QMenu(tr("&Commandes"));
    QAction *showCommandBarAction = commandMenu->addAction(tr("Afficher/Masquer la barre de commande"));
    //showCommandBarAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    //showCommandBarAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
    showCommandBarAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Slash));
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
    downloadsAction->setIcon(QIcon(":/icons/download.png"));
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
    QFile file("src/favorites/favorites.json");

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
    QFile file("src/favorites/favorites.json");
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
    m_favoritesBar->setMovable(true);
    m_favoritesBar->setAcceptDrops(true);
    m_favoritesBar->setContextMenuPolicy(Qt::CustomContextMenu);
    m_favoritesBar->installEventFilter(this);

    connect(m_favoritesBar, &QToolBar::customContextMenuRequested, this, &BrowserWindow::showFavoriteContextMenu);

    // Gestion du drag-drop manuel
    m_favoritesBar->setAcceptDrops(true);

     // Ajouter le bouton "Voir plus" seulement s'il y a des favoris
     if (!m_favoritesRoot->children.isEmpty()) {
        m_moreFavoritesAction = new QAction(QIcon(":/icons/more.png"), tr("Voir plus..."), this);
        connect(m_moreFavoritesAction, &QAction::triggered, this, &BrowserWindow::showFavoritesManager);
        m_favoritesBar->addAction(m_moreFavoritesAction);
    }
}

bool BrowserWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_favoritesBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QAction *action = m_favoritesBar->actionAt(mouseEvent->pos());
            if (action && action != m_moreFavoritesAction) {
                m_draggedIndex = m_favoritesBar->actions().indexOf(action);
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove && m_draggedIndex != -1) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                // Logique de déplacement visuel ici
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QAction *targetAction = m_favoritesBar->actionAt(mouseEvent->pos());
            if (targetAction && m_draggedIndex != -1) {
                int newIndex = m_favoritesBar->actions().indexOf(targetAction);
                
                // Mise à jour de la structure de données
                if (newIndex != -1 && newIndex != m_draggedIndex) {
                    m_favoritesRoot->children.move(m_draggedIndex, newIndex);
                    loadFavoritesToBar();
                }
                m_draggedIndex = -1;
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}



void BrowserWindow::handleFavoriteDrop(QDropEvent *event) {
    if (m_draggedIndex == -1) return;

    QPoint pos = event->position().toPoint();
    int newIndex = m_favoritesBar->actions().indexOf(m_favoritesBar->actionAt(pos));

    if (newIndex != -1 && newIndex != m_draggedIndex) {
        // Échanger les éléments dans la structure de données
        m_favoritesRoot->children.swapItemsAt(m_draggedIndex, newIndex);
        
        // Mettre à jour l'interface
        loadFavoritesToBar();
    }
    
    m_draggedIndex = -1;
}

void BrowserWindow::loadFavoritesToBar() {
    m_favoritesBar->clear();

    std::function<void(FavoriteItem*, QWidget*)> addFavoritesToBar;
    addFavoritesToBar = [this, &addFavoritesToBar](FavoriteItem* item, QWidget* parent) {
        if (item->url.isEmpty()) {
            // C'est un dossier
            QMenu* folderMenu = new QMenu(item->title, parent);
            folderMenu->setIcon(QIcon(":/icons/folder.png"));
            for (FavoriteItem* child : item->children) {
                addFavoritesToBar(child, folderMenu);
            }
            if (parent == m_favoritesBar) {
                m_favoritesBar->addAction(folderMenu->menuAction());
            } else {
                static_cast<QMenu*>(parent)->addMenu(folderMenu);
            }
        } else {
            // C'est un favori
            QAction* action = new QAction(item->title, parent);
            action->setIcon(QIcon(item->iconPath.isEmpty() ? ":/icons/favicon.png" : item->iconPath));
            action->setData(QUrl(item->url));
            connect(action, &QAction::triggered, this, [this, url = QUrl(item->url)]() {
                m_tabWidget->setUrl(url);
            });
            if (parent == m_favoritesBar) {
                m_favoritesBar->addAction(action);
            } else {
                static_cast<QMenu*>(parent)->addAction(action);
            }
        }
    };

    for (FavoriteItem* item : m_favoritesRoot->children) {
        addFavoritesToBar(item, m_favoritesBar);
    }

    // Ajouter le bouton "Voir plus" seulement s'il y a des favoris
    if (!m_favoritesRoot->children.isEmpty()) {
        m_moreFavoritesAction = new QAction(QIcon(":/icons/more.png"), tr("Voir plus..."), this);
        connect(m_moreFavoritesAction, &QAction::triggered, this, &BrowserWindow::showFavoritesManager);
        m_favoritesBar->addAction(m_moreFavoritesAction);
    }
}




void BrowserWindow::startDrag()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) return;

    m_draggedIndex = m_favoritesBar->actions().indexOf(action);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(action->text());
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}

void BrowserWindow::addFavoriteToBar(FavoriteItem* item, QWidget* parent)
{
    if (item->url.isEmpty()) {
        // Gestion des dossiers
        QMenu* folderMenu = new QMenu(item->title, parent);
        folderMenu->setIcon(QIcon(":/3rdparty/folder-closed.png"));

        for (FavoriteItem* child : item->children) {
            addFavoriteToBar(child, folderMenu);
        }

        if (folderMenu->isEmpty()) {
            QAction* emptyAction = new QAction(tr("Dossier vide"), folderMenu);
            emptyAction->setEnabled(false);
            folderMenu->addAction(emptyAction);
        }

        if (QToolBar* tb = qobject_cast<QToolBar*>(parent)) {
            tb->addAction(folderMenu->menuAction());
        }
    } else {
        // C'est un favori
        QAction* action = new QAction(parent);
        action->setText(item->title);
        action->setToolTip(item->url);

        if (!item->iconPath.isEmpty()) {
            action->setIcon(QIcon(item->iconPath));
        } else {
            // Générer une icône à partir de la première lettre
            QPixmap pix(16, 16);
            pix.fill(Qt::transparent);
            QPainter painter(&pix);
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(pix.rect(), Qt::AlignCenter, item->title.left(1).toUpper());
            action->setIcon(QIcon(pix));
        }
        action->setData(QUrl(item->url));

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

void BrowserWindow::showFavoriteContextMenu(const QPoint &pos)
{
    QAction *clickedAction = m_favoritesBar->actionAt(pos);
    QUrl currentUrl = currentTab()->url();
    QString currentTitle = currentTab()->title();

    QMenu contextMenu(this);

    QAction *addFolderAction = contextMenu.addAction(tr("Créer un dossier"));
    connect(addFolderAction, &QAction::triggered, this, [this]() {
        bool ok;
        QString folderName = QInputDialog::getText(this, tr("Nouveau dossier"),
        tr("Nom du dossier:"), QLineEdit::Normal, "", &ok);
        if (ok && !folderName.isEmpty()) {
            // Ajouter le dossier à la base de données
            int newFolderId = m_database.addFavorite(folderName, "", "", 0);
            if (newFolderId != -1) {
                loadFavoritesFromDatabase();
                loadFavoritesToBar();
            }
        }
    });

    if (clickedAction && clickedAction != m_moreFavoritesAction) {
        // Mode Édition pour un favori existant
        QUrl actionUrl = clickedAction->data().toUrl();
        QString actionTitle = clickedAction->text();
        
        QAction *editAction = contextMenu.addAction(tr("✎ Modifier"));
        QAction *deleteAction = contextMenu.addAction(tr("🗑 Supprimer"));
        QAction *moveAction = contextMenu.addAction(tr("➔ Déplacer vers..."));

        connect(deleteAction, &QAction::triggered, [this, actionUrl]() {
            deleteFavorite(actionUrl);
        });

        connect(editAction, &QAction::triggered, [this, actionUrl]() {
            FavoriteItem* item = findFavoriteByUrl(actionUrl);
            if(item) editFavorite(item);
        });

        connect(moveAction, &QAction::triggered, [this, actionUrl]() {
            QDialog moveDialog(this);
            QVBoxLayout layout(&moveDialog);
            
            QTreeWidget folderTree;
            folderTree.setHeaderLabel(tr("Dossiers"));
            populateFolderTree(&folderTree, m_favoritesRoot);
            
            QPushButton newFolderBtn(tr("Nouveau dossier"));
            connect(&newFolderBtn, &QPushButton::clicked, [&]() {
                bool ok;
                QString name = QInputDialog::getText(&moveDialog, tr("Nouveau dossier"), 
                                                    tr("Nom:"), QLineEdit::Normal, "", &ok);
                if(ok && !name.isEmpty()) {
                    FavoriteItem* newFolder = new FavoriteItem{-1, name, "", "", {}, m_favoritesRoot};
                    m_favoritesRoot->children.append(newFolder);
                    
                    // Ajouter au tree widget
                    QTreeWidgetItem* treeItem = new QTreeWidgetItem({name});
                    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(newFolder));
                    folderTree.invisibleRootItem()->addChild(treeItem);
                }
            });

            layout.addWidget(&folderTree);
            layout.addWidget(&newFolderBtn);
            
            QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
            layout.addWidget(&buttons);
            
            connect(&buttons, &QDialogButtonBox::accepted, &moveDialog, &QDialog::accept);
            connect(&buttons, &QDialogButtonBox::rejected, &moveDialog, &QDialog::reject);

            if(moveDialog.exec() == QDialog::Accepted) {
                FavoriteItem* targetFolder = getSelectedFolder(&folderTree);
                FavoriteItem* item = findFavoriteByUrl(actionUrl);
                if(item && targetFolder) {
                    // Retirer de l'ancien parent
                    if(item->parent) item->parent->children.removeAll(item);
                    // Ajouter au nouveau parent
                    item->parent = targetFolder;
                    targetFolder->children.append(item);
                    loadFavoritesToBar();
                }
            }
        });

    } else {
        // Mode Ajout d'un nouveau favori
        QAction *addAction = contextMenu.addAction(tr("⭐ Ajouter aux favoris"));
        
        connect(addAction, &QAction::triggered, [this, currentUrl, currentTitle]() {
            QDialog dialog(this);
            dialog.setWindowTitle(tr("Nouveau favori"));
            QFormLayout form(&dialog);
            
            // Widgets de saisie
            QLineEdit titleEdit(currentTitle);
            QLineEdit urlEdit(currentUrl.toString());
            QTreeWidget folderTree;
            folderTree.setHeaderLabel(tr("Dossiers"));
            populateFolderTree(&folderTree, m_favoritesRoot);
            
            // Bouton nouveau dossier
            QPushButton newFolderBtn(tr("Nouveau dossier"));
            connect(&newFolderBtn, &QPushButton::clicked, [&]() {
                bool ok;
                QString name = QInputDialog::getText(&dialog, tr("Nouveau dossier"), 
                                                    tr("Nom:"), QLineEdit::Normal, "", &ok);
                if(ok && !name.isEmpty()) {
                    FavoriteItem* newFolder = new FavoriteItem{-1, name, "", "", {}, m_favoritesRoot};
                    m_favoritesRoot->children.append(newFolder);
                    
                    // Mettre à jour l'arborescence
                    QTreeWidgetItem* treeItem = new QTreeWidgetItem({name});
                    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(newFolder));
                    folderTree.invisibleRootItem()->addChild(treeItem);
                }
            });

            form.addRow(tr("Titre:"), &titleEdit);
            form.addRow(tr("URL:"), &urlEdit);
            form.addRow(tr("Dossier:"), &folderTree);
            form.addRow(&newFolderBtn);
            
            QDialogButtonBox buttons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
            form.addRow(&buttons);

            connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            if(dialog.exec() == QDialog::Accepted) {
                FavoriteItem* selectedFolder = getSelectedFolder(&folderTree);
                if(selectedFolder) {
                    addFavoriteToFolder(titleEdit.text(), urlEdit.text(), selectedFolder);
                }
            }
        });
    }

    contextMenu.exec(m_favoritesBar->mapToGlobal(pos));
}

void BrowserWindow::editFavorite(FavoriteItem* item)
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Modifier le favori"));
    QFormLayout form(&dialog);

    QLineEdit titleEdit(item->title);
    QLineEdit urlEdit(item->url);
    
    QTreeWidget folderTree;
    folderTree.setHeaderLabel(tr("Dossiers"));
    populateFolderTree(&folderTree, m_favoritesRoot);
    
    // Sélectionner le dossier actuel
    QTreeWidgetItem* currentFolderItem = findTreeItem(&folderTree, item->parent);
    if(currentFolderItem) folderTree.setCurrentItem(currentFolderItem);

    QPushButton newFolderBtn(tr("Nouveau dossier"));
    connect(&newFolderBtn, &QPushButton::clicked, [&]() {
        bool ok;
        QString name = QInputDialog::getText(&dialog, tr("Nouveau dossier"), 
                                          tr("Nom:"), QLineEdit::Normal, "", &ok);
        if(ok && !name.isEmpty()) {
            FavoriteItem* newFolder = new FavoriteItem{-1, name, "", "", {}, m_favoritesRoot};
            m_favoritesRoot->children.append(newFolder);
            
            QTreeWidgetItem* treeItem = new QTreeWidgetItem({name});
            treeItem->setData(0, Qt::UserRole, QVariant::fromValue(newFolder));
            folderTree.invisibleRootItem()->addChild(treeItem);
        }
    });

    QPushButton deleteBtn(tr("Supprimer"));
    connect(&deleteBtn, &QPushButton::clicked, [&]() {
        deleteFavorite(QUrl(item->url));
        dialog.reject();
    });

    form.addRow(tr("Titre:"), &titleEdit);
    form.addRow(tr("URL:"), &urlEdit);
    form.addRow(tr("Dossier:"), &folderTree);
    form.addRow(&newFolderBtn);
    
    QDialogButtonBox buttons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    form.addRow(&deleteBtn);

    if(dialog.exec() == QDialog::Accepted) {
        item->title = titleEdit.text();
        item->url = urlEdit.text();
        
        // Déplacer dans le nouveau dossier
        FavoriteItem* targetFolder = getSelectedFolder(&folderTree);
        if(targetFolder && item->parent != targetFolder) {
            item->parent->children.removeAll(item);
            item->parent = targetFolder;
            targetFolder->children.append(item);
        }
        
        loadFavoritesToBar();
    }
}

QTreeWidgetItem* BrowserWindow::findTreeItem(QTreeWidget* tree, FavoriteItem* target)
{
    QTreeWidgetItemIterator it(tree);
    while(*it) {
        if((*it)->data(0, Qt::UserRole).value<FavoriteItem*>() == target)
            return *it;
        ++it;
    }
    return nullptr;
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
            addFavorite(name, url, "", 0); // Spécifier tous les paramètres
        }
    }
}

FavoriteItem* BrowserWindow::getSelectedFolderFromTree(QTreeWidget* tree)
{
    QTreeWidgetItem* selected = tree->currentItem();
    return selected ? selected->data(0, Qt::UserRole).value<FavoriteItem*>() : m_favoritesRoot;
}




bool BrowserWindow::addFavorite(const QString &name, const QString &url, const QString &iconPath, int parentId)
{
    bool success = m_database.addFavorite(name, url, iconPath, parentId);
    if (success) {
        loadFavoritesFromDatabase();
        loadFavoritesToBar();
    }
    return success;
}


bool BrowserWindow::deleteFavorite(const QUrl& url)
{
    auto favoriteData = m_database.getFavoriteByUrl(url);
    if(!favoriteData.isEmpty()) {
        return m_database.deleteFavorite(favoriteData["id"].toInt());
    }
    return false;
}



void BrowserWindow::handleFavActionTriggered()
{
    WebView* view = currentTab();
    if(!view) return;

    QUrl url = view->url();
    QString title = view->title();
    QIcon favicon = view->favIcon();

    if (isFavorite(url)) {
        // Supprimer le favori
        if (m_database.deleteFavorite(m_database.getFavoriteByUrl(url)["id"].toInt())) {
            loadFavoritesFromDatabase();
            loadFavoritesToBar();
        }
    } else {
        // Ajouter le favori
        if (m_database.addFavorite(title, url.toString(), "", 0) != -1) {
            loadFavoritesFromDatabase();
            loadFavoritesToBar();
        }
    }
    updateFavoriteIcon(url);

    // auto favoriteData = m_database.getFavoriteByUrl(url);
    
    // if(!favoriteData.isEmpty()) {
    //     int ret = QMessageBox::question(this, tr("Supprimer le favori"),
    //                                   tr("Voulez-vous vraiment supprimer ce favori ?"),
    //                                   QMessageBox::Yes | QMessageBox::No);
    //     if(ret == QMessageBox::Yes && m_database.deleteFavorite(favoriteData["id"].toInt())) {
    //         loadFavoritesFromDatabase();
    //         m_favAction->setIcon(QIcon(":/star-regular.png"));
    //     }
    // } else {
    //     bool ok;
    //     QString name = QInputDialog::getText(this, tr("Ajouter aux favoris"),
    //                                        tr("Nom du favori:"), QLineEdit::Normal,
    //                                        title, &ok);
    //     if(ok && !name.isEmpty()) {
    //         QByteArray iconData;
    //         QBuffer buffer(&iconData);
    //         favicon.pixmap(16,16).save(&buffer, "PNG");
    //         QString iconPath = saveFavicon(iconData, url);
    //         if(m_database.addFavorite(name, url.toString(), iconPath)) {
    //             loadFavoritesFromDatabase();
    //             m_favAction->setIcon(QIcon(":/star-solid.png"));
    //         }
    //     }
    // }
    // updateFavoriteIcon(url);
}


// bool BrowserWindow::isFavorite(const QUrl &url) const
// {
//     qDebug() << "Vérification si l'URL est un favori:" << url.toString();
//     QFile file("src/favorites/favorites.json");
//     if (file.open(QIODevice::ReadOnly)) {
//         QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
//         QJsonArray favoritesArray = doc.array();
//         file.close();
//         qDebug() << "Nombre de favoris trouvés:" << favoritesArray.size();
//         for (const QJsonValue &value : favoritesArray) {
//             if (value.toObject()["url"].toString() == url.toString()) {
//                 qDebug() << "Favori trouvé pour l'URL:" << url.toString();
//                 return true;
//             }
//         }
//     } else {
//         qWarning() << "Impossible d'ouvrir le fichier favorites.json";
//     }
//     qDebug() << "URL non trouvée dans les favoris";
//     return false;
// }

bool BrowserWindow::isFavorite(const QUrl &url) const
{
    auto favoriteData = m_database.getFavoriteByUrl(url);
    return !favoriteData.isEmpty();
}


bool BrowserWindow::updateFavorite(int id, const QString &newTitle, const QString &newUrl, int parentId)
{
    return m_database.updateFavorite(id, newTitle, newUrl, parentId);
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
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cachePath);
    
    QString fileName = cachePath + "/" + url.host() + ".ico";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    }
    return fileName;
}

void BrowserWindow::updateFaviconForFavorite(const QUrl &url, const QString &faviconPath)
{
    QFile file("src/favorites/favorites.json");
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


void BrowserWindow::showFavoritesManager() {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Gestionnaire de favoris"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QTreeWidget *tree = new QTreeWidget(&dialog);
    tree->setHeaderLabels({tr("Nom"), tr("URL")});
    populateFavoritesTree(tree, m_favoritesRoot);

    QPushButton *addFolderButton = new QPushButton(tr("Ajouter un dossier"), &dialog);
    connect(addFolderButton, &QPushButton::clicked, [this, tree]() {
        bool ok;
        QString folderName = QInputDialog::getText(this, tr("Nouveau dossier"),
                                                   tr("Nom du dossier:"), QLineEdit::Normal, "", &ok);
        if (ok && !folderName.isEmpty()) {
            addFavoriteFolder(folderName);
            populateFavoritesTree(tree, m_favoritesRoot);
        }
    });

    layout->addWidget(tree);
    layout->addWidget(addFolderButton);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        saveFavoritesFromTree(tree);
        loadFavoritesToBar();
    }
}


void BrowserWindow::addFavoriteToFolder(const QString &name, const QString &url, FavoriteItem* folder) {
    int newId = m_database.addFavorite(name, url, "", folder ? folder->id : 0);
    if (newId != -1) {
        FavoriteItem* newItem = new FavoriteItem(newId, name, url, "", {}, folder);
        folder->children.append(newItem);
        loadFavoritesToBar();
    }
}


void BrowserWindow::populateFolderTree(QTreeWidget* tree, FavoriteItem* parent) {
    tree->clear();
    if (!parent) parent = m_favoritesRoot;
    
    std::function<void(QTreeWidgetItem*, FavoriteItem*)> addItems;
    addItems = [&](QTreeWidgetItem* parentItem, FavoriteItem* favorite) {
        for (FavoriteItem* child : favorite->children) {
            if (child->url.isEmpty()) {  // C'est un dossier
                QTreeWidgetItem* item = new QTreeWidgetItem({child->title});
                item->setData(0, Qt::UserRole, QVariant::fromValue(child));
                if (parentItem) {
                    parentItem->addChild(item);
                } else {
                    tree->addTopLevelItem(item);
                }
                addItems(item, child);
            }
        }
    };
    
    addItems(nullptr, parent);
}



void BrowserWindow::saveFavoritesFromTree(QTreeWidget *tree)
{
    QFile file("src/favorites/favorites.json");
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

    WebView *view = currentTab();
    if (!view) {
        qWarning() << "WebView non initialisé !";
        return;
    }
    if (!m_favAction) {
        qWarning() << "m_favAction non initialisé !";
        return;
    }

    bool isFav = isFavorite(url);
    QColor iconColor = isFav ? Qt::blue : Qt::white; // Couleur selon l'état
    
    // Créez une icône avec la lettre "F" en fonction de l'état
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setPen(QPen(iconColor));
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(pix.rect(), Qt::AlignCenter, "F");
    m_favAction->setIcon(QIcon(pix));
    // Mettre à jour le menu contextuel
    m_favAction->setMenu(isFav ? createFavoriteContextMenu(url) : nullptr);
}

QMenu* BrowserWindow::createFavoriteContextMenu(const QUrl &url)
{
    QMenu* menu = new QMenu(this);
    QAction* editAction = menu->addAction(tr("Modifier le favori"));
    QAction* deleteAction = menu->addAction(tr("Supprimer le favori"));

    connect(editAction, &QAction::triggered, this, [this, url]() {
        if (FavoriteItem* item = findFavoriteByUrl(url))
            editFavorite(item); // Passez l'objet FavoriteItem directement
    });

    connect(deleteAction, &QAction::triggered, this, [this, url]() {
        deleteFavorite(url);
    });

    return menu;

}



void BrowserWindow::updateUrlCompleter()
{
    QStringList urls;
    QFile file("src/favorites/favorites.json");
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

void BrowserWindow::loadFavoritesFromDatabase()
{
    delete m_favoritesRoot;
    m_favoritesRoot = new FavoriteItem(-1, "Root", "", "", {}, nullptr);
    
    QVector<QMap<QString, QVariant>> favorites = m_database.getFavorites(0);
    QHash<int, FavoriteItem*> items;

    // Création des items
    for (const auto &fav : favorites) {
        items[fav["id"].toInt()] = new FavoriteItem(
            fav["id"].toInt(),
            fav["title"].toString(),
            fav["url"].toString(),
            fav["icon_path"].toString(),
            {},
            nullptr
        );
    }

    // Construction hiérarchique
    for (const auto &fav : favorites) {
        int parentId = fav["parent_id"].toInt();
        if (parentId > 0 && items.contains(parentId)) {
            items[parentId]->children.append(items[fav["id"].toInt()]);
            items[fav["id"].toInt()]->parent = items[parentId];
        } else {
            m_favoritesRoot->children.append(items[fav["id"].toInt()]);
        }
    }
    
    loadFavoritesToBar(); // Rafraîchir l'UI
}


void BrowserWindow::showCommandPalette()
{
    if (!m_commandPalette->isVisible()) {
        m_commandPalette->showPalette();
    }
}


void BrowserWindow::onCommandPaletteCommandSelected(const QString &command)
{
    // Passer la commande à CommandPalette pour le traitement
    m_commandPalette->processCommand(command);
}


FavoriteItem* BrowserWindow::findFavoriteByUrl(const QUrl& url, FavoriteItem* root)
{
    if (!root) root = m_favoritesRoot;
    
    if (root->url == url.toString()) {
        return root;
    }
    
    for (FavoriteItem* child : root->children) {
        FavoriteItem* result = findFavoriteByUrl(url, child);
        if (result) {
            return result;
        }
    }
    
    return nullptr;
}

void BrowserWindow::duplicateCurrentTab()
{
    WebView *currentView = currentTab();
    if (currentView) {
        WebView *newView = m_tabWidget->createTab();
        newView->setUrl(currentView->url());
        m_tabWidget->setCurrentWidget(newView);
    }
}