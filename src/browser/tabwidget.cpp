#include "tabwidget.h"
#include "webpage.h"
#include "webview.h"
#include <QLabel>
#include <QMenu>
#include <QTabBar>
#include <QWebEngineProfile>
#include <QInputDialog>

using namespace Qt::StringLiterals;

TabWidget::TabWidget(QWebEngineProfile *profile, QWidget *parent)
    : QTabWidget(parent)
    , m_profile(profile)
{
    QTabBar *tabBar = this->tabBar();
    tabBar->setTabsClosable(true);
    tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    tabBar->setMovable(true);
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar, &QTabBar::customContextMenuRequested, this, &TabWidget::handleContextMenuRequested);
    connect(tabBar, &QTabBar::tabCloseRequested, [this](int index) {
        if (WebView *view = webView(index))
            view->page()->triggerAction(QWebEnginePage::WebAction::RequestClose);
    });
    connect(tabBar, &QTabBar::tabBarDoubleClicked, [this](int index) {
        if (index == -1)
            createTab();
    });

    setDocumentMode(true);
    setElideMode(Qt::ElideRight);

    connect(this, &QTabWidget::currentChanged, this, &TabWidget::handleCurrentChanged);

    if (profile->isOffTheRecord()) {
        QLabel *icon = new QLabel(this);
        QPixmap pixmap(u":ninja.png"_s);
        icon->setPixmap(pixmap.scaledToHeight(tabBar->height()));
        setStyleSheet(u"QTabWidget::tab-bar { left: %1px; }"_s.
                      arg(icon->pixmap().width()));
    }
}

void TabWidget::handleCurrentChanged(int index)
{
    if (index != -1) {
        WebView *view = webView(index);
        if (!view->url().isEmpty())
            view->setFocus();
        emit titleChanged(view->title());
        emit loadProgress(view->loadProgress());
        emit urlChanged(view->url());
        emit favIconChanged(view->favIcon());
        emit webActionEnabledChanged(QWebEnginePage::Back, view->isWebActionEnabled(QWebEnginePage::Back));
        emit webActionEnabledChanged(QWebEnginePage::Forward, view->isWebActionEnabled(QWebEnginePage::Forward));
        emit webActionEnabledChanged(QWebEnginePage::Stop, view->isWebActionEnabled(QWebEnginePage::Stop));
        emit webActionEnabledChanged(QWebEnginePage::Reload,view->isWebActionEnabled(QWebEnginePage::Reload));
    } else {
        emit titleChanged(QString());
        emit loadProgress(0);
        emit urlChanged(QUrl());
        emit favIconChanged(QIcon());
        emit webActionEnabledChanged(QWebEnginePage::Back, false);
        emit webActionEnabledChanged(QWebEnginePage::Forward, false);
        emit webActionEnabledChanged(QWebEnginePage::Stop, false);
        emit webActionEnabledChanged(QWebEnginePage::Reload, true);
    }
}

void TabWidget::handleContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    menu.addAction(tr("New &Tab"), QKeySequence::AddTab, this, &TabWidget::createTab);
#else
    menu.addAction(tr("New &Tab"), this, &TabWidget::createTab, QKeySequence::AddTab);
#endif
    int index = tabBar()->tabAt(pos);
    if (index != -1) {
        menu.addSeparator();
        QAction *createGroupAction = menu.addAction(tr("Créer un groupe"));
        connect(createGroupAction, &QAction::triggered, this, &TabWidget::createTabGroup);


        if (!tabBar()->tabData(index).isNull()) {
            QAction *removeFromGroupAction = menu.addAction(tr("Retirer du groupe"));
            connect(removeFromGroupAction, &QAction::triggered, this, [this, index]() {
                removeFromGroup(index);
            });
        }
    }
    menu.exec(QCursor::pos());
}

WebView *TabWidget::currentWebView() const
{
    return webView(currentIndex());
}

WebView *TabWidget::webView(int index) const
{
    return qobject_cast<WebView*>(widget(index));
}

void TabWidget::setupView(WebView *webView)
{
    QWebEnginePage *webPage = webView->page();

    connect(webView, &QWebEngineView::titleChanged, [this, webView](const QString &title) {
        int index = indexOf(webView);
        if (index != -1) {
            setTabText(index, title);
            setTabToolTip(index, title);
        }
        if (currentIndex() == index)
            emit titleChanged(title);
    });
    connect(webView, &QWebEngineView::urlChanged, [this, webView](const QUrl &url) {
        int index = indexOf(webView);
        if (index != -1)
            tabBar()->setTabData(index, url);
        if (currentIndex() == index)
            emit urlChanged(url);
    });
    connect(webView, &QWebEngineView::loadProgress, [this, webView](int progress) {
        if (currentIndex() == indexOf(webView))
            emit loadProgress(progress);
    });
    connect(webPage, &QWebEnginePage::linkHovered, [this, webView](const QString &url) {
        if (currentIndex() == indexOf(webView))
            emit linkHovered(url);
    });
    connect(webView, &WebView::favIconChanged, [this, webView](const QIcon &icon) {
        int index = indexOf(webView);
        if (index != -1)
            setTabIcon(index, icon);
        if (currentIndex() == index)
            emit favIconChanged(icon);
    });
    connect(webView, &WebView::favIconChanged, this, &TabWidget::updateFavicon);
    connect(webView, &WebView::webActionEnabledChanged, [this, webView](QWebEnginePage::WebAction action, bool enabled) {
        if (currentIndex() ==  indexOf(webView))
            emit webActionEnabledChanged(action,enabled);
    });
    connect(webPage, &QWebEnginePage::windowCloseRequested, [this, webView]() {
        int index = indexOf(webView);
        if (webView->page()->inspectedPage())
            window()->close();
        else if (index >= 0)
            closeTab(index);
    });
    connect(webView, &WebView::devToolsRequested, this, &TabWidget::devToolsRequested);
    connect(webPage, &QWebEnginePage::findTextFinished, [this, webView](const QWebEngineFindTextResult &result) {
        if (currentIndex() == indexOf(webView))
            emit findTextFinished(result);
    });
}

WebView *TabWidget::createTab()
{
    WebView *webView = createBackgroundTab();
    setCurrentWidget(webView);
    return webView;
}

WebView *TabWidget::createBackgroundTab()
{
    WebView *webView = new WebView;
    WebPage *webPage = new WebPage(m_profile, webView);
    webView->setPage(webPage);
    setupView(webView);
    int index = addTab(webView, tr("(Untitled)"));
    setTabIcon(index, webView->favIcon());
    // Workaround for QTBUG-61770
    webView->resize(currentWidget()->size());
    webView->show();
    return webView;
}

void TabWidget::reloadAllTabs()
{
    for (int i = 0; i < count(); ++i)
        webView(i)->reload();
}

void TabWidget::closeOtherTabs(int index)
{
    for (int i = count() - 1; i > index; --i)
        closeTab(i);
    for (int i = index - 1; i >= 0; --i)
        closeTab(i);
}

void TabWidget::closeTab(int index)
{
    if (WebView *view = webView(index)) {
        bool hasFocus = view->hasFocus();
        removeTab(index);
        if (hasFocus && count() > 0)
            currentWebView()->setFocus();
        if (count() == 0)
            createTab();
        view->deleteLater();
    }
}

void TabWidget::cloneTab(int index)
{
    if (WebView *view = webView(index)) {
        WebView *tab = createTab();
        tab->setUrl(view->url());
    }
}

void TabWidget::setUrl(const QUrl &url)
{
    if (WebView *view = currentWebView()) {
        view->setUrl(url);
        view->setFocus();
    }
}

void TabWidget::triggerWebPageAction(QWebEnginePage::WebAction action)
{
    if (WebView *webView = currentWebView()) {
        webView->triggerPageAction(action);
        webView->setFocus();
    }
}

void TabWidget::nextTab()
{
    int next = currentIndex() + 1;
    if (next == count())
        next = 0;
    setCurrentIndex(next);
}

void TabWidget::previousTab()
{
    int next = currentIndex() - 1;
    if (next < 0)
        next = count() - 1;
    setCurrentIndex(next);
}

void TabWidget::reloadTab(int index)
{
    if (WebView *view = webView(index))
        view->reload();
}

void TabWidget::createTabGroup()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Créer un groupe d'onglets"));
    dialog.setLabelText(tr("Nom du groupe :"));
    dialog.setInputMode(QInputDialog::TextInput);

    if (dialog.exec() == QDialog::Accepted) {
        QString groupName = dialog.textValue();
        if (!groupName.isEmpty()) {
            int currentIndex = this->currentIndex();
            tabBar()->setTabTextColor(currentIndex, QColor(Qt::blue));
            tabBar()->setTabData(currentIndex, groupName);
        }
    }
}

void TabWidget::addToGroup(int index, const QString &groupName)
{
    tabBar()->setTabTextColor(index, QColor(Qt::blue));
    tabBar()->setTabData(index, groupName);
}

void TabWidget::removeFromGroup(int index)
{
    tabBar()->setTabTextColor(index, QColor());
    tabBar()->setTabData(index, QVariant());
}


void TabWidget::handleWebViewTitleChanged(const QString &title)
{
    WebView *view = qobject_cast<WebView*>(sender());
    if (view) {
        int index = indexOf(view);
        QString shortTitle = title.length() > 30 ? title.left(17) + "..." : title;
        setTabText(index, shortTitle);
        setTabToolTip(index, title);
    }
}


void TabWidget::updateFavicon(const QIcon &icon)
{
    WebView *view = qobject_cast<WebView*>(sender());
    if (view) {
        int index = indexOf(view);
        if (index != -1) {
            setTabIcon(index, icon);
            emit favIconChanged(icon); // Pour mettre à jour la barre de favoris
        }
    }
}
