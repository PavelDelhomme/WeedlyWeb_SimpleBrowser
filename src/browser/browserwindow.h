#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEnginePage>
#include <QToolBar>
#include <QProgressBar>
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
#include <QLineEdit>
#include <QPainter>
#include <QDrag>
#include <QMimeData>

#include "commandwidget.h"
#include "requestinterceptor.h"
#include "database.h"

class Browser;
class TabWidget;
class WebView;

struct FavoriteItem {
    int id; // <-- Ajouter ceci
    QString title;
    QString url;
    QString iconPath;
    QList<FavoriteItem*> children;
    FavoriteItem* parent;
    
    // Constructeur
    FavoriteItem(int id = -1, QString title = "", QString url = "", QString iconPath = "", 
                 QList<FavoriteItem*> children = {}, FavoriteItem* parent = nullptr)
        : id(id), title(title), url(url), iconPath(iconPath), children(children), parent(parent) {}
};


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
    void refreshFavoriteIcon(const QUrl &url) { updateFavoriteIcon(url); }
    void ensureFavoritesFileExists();
    void serializeFavoriteTree(FavoriteItem* root, QJsonArray& array);

    void buildFavoriteTree(const QJsonArray& array, FavoriteItem* parent);

    bool isFavorite(const QUrl &url) const;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

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
    void populateFolderTree(QTreeWidget* tree, FavoriteItem* parent = nullptr);
    void handleFavoriteDrop(QDropEvent *event);
    void startDrag();
    void processCommand(const QString &command);
    void toggleCommandWidget();

private:
    QMenu *createFileMenu(TabWidget *tabWidget);
    QMenu *createEditMenu();
    QMenu *createViewMenu(QToolBar *toolBar);
    QMenu *createWindowMenu(TabWidget *tabWidget);
    QMenu *createHelpMenu();
    QToolBar *createToolBar();
    QMenu *createCommandMenu();

private:
    Browser *m_browser;
    QWebEngineProfile *m_profile;
    TabWidget *m_tabWidget;
    CommandWidget *m_commandWidget;
    QProgressBar *m_progressBar = nullptr;
    QAction *m_historyBackAction = nullptr;
    QAction *m_historyForwardAction = nullptr;
    QAction *m_stopAction = nullptr;
    QAction *m_reloadAction = nullptr;
    QAction *m_stopReloadAction = nullptr;
    QCompleter *m_urlCompleter;
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
    Database m_database;
    int m_draggedIndex = -1; // Ajoutez cette ligne

    FavoriteItem* m_favoritesRoot;
    void addFavoriteToBar(FavoriteItem* item, QWidget* parent);
    FavoriteItem* findFavoriteByUrl(const QUrl& url, FavoriteItem* root=nullptr);
    FavoriteItem* getSelectedFolder(QTreeWidget* tree);
    FavoriteItem* getSelectedFolderFromTree(QTreeWidget* tree);


    // Fonctions
    // // Favorite
    void setupFavoritesBar();
    void setupFavoritesMenu();
    void addCurrentPageToFavorites();

    // Crud
    bool addFavorite(const QString &name, const QString &url, const QString &iconPath, int parentId = 0);
    bool deleteFavorite(int id);
    bool updateFavorite(int id, const QString &newTitle, const QString &newUrl, int parentId);

    void updateFaviconInFavoritesBar(const QUrl &url, const QIcon &icon);
    void addFavoriteToFolder(const QString &name, const QString &url, FavoriteItem* folder);
    QMenu* createFavoriteContextMenu(const QUrl &url);

    void loadFavoritesFromDatabase();
    void loadFavoritesToBar();
    void openFavorite(const QUrl &url);
    void saveFavorite(const QUrl &url, const QString &title);
    void loadFavorites();
    void loadFavoritesToBarRecursive(const QJsonArray& array, QWidget* parent);

    void showFavoriteContextMenu(const QPoint &pos);
    void editFavorite(FavoriteItem* item);
    QString saveFavicon(const QByteArray &data, const QUrl &url);
    void updateFaviconForFavorite(const QUrl &url, const QString &faviconPath);
    void updateFavoriteIcon(const QUrl &url, bool ok = true);
    QTreeWidgetItem* findTreeItem(QTreeWidget* tree, FavoriteItem* target);

    // Others
    void handleWebViewLoadFinished(bool ok);
    void updateUrlCompleter();

    // Command
    void processCVECommand(const QString &command);
    void processRequestCommand(const QString &command);

    // Command - CVEs
    QStringList detectCVEs(const QString &html);
    void displayCVEResults(const QStringList &cves);
    
    // Command - Request
    RequestInterceptor *m_requestInterceptor;
    void showRequestAnalyzer();
    void sendGetRequest(const QString &url);
    void sendPostRequest(const QString &url);
    void handleNetworkReply(QNetworkReply *reply);

};

#endif // BROWSERWINDOW_H
