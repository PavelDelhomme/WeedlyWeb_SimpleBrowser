#ifndef FAVORITESMANAGER_H
#define FAVORITESMANAGER_H

#include <QDialog>
#include <QTreeView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QDropEvent>
#include <QMenu>
#include "browserwindow.h"
#include "favoriteitem.h"

class FavoritesManager : public QDialog
{
    Q_OBJECT;

public:
    explicit FavoritesManager(QWidget *parent = nullptr);
    void showContextMenu(const QPoint &pos);
    void updateTreeView(FavoriteItem* root);

protected:
    void dropEvent(QDropEvent *event) override;

private slots:
    void addFolder();
    void addFavorite();
    void deleteFavorite();
    void editFavorite();
    void saveFavorites();

private:
    QTreeView *m_favoritesTree;
    QStandardItemModel *m_favoritesModel;
    FavoriteItem* m_favoritesRoot;
    QLineEdit *m_nameEdit;
    QLineEdit *m_urlEdit;
    QLineEdit *m_tagsEdit;

    void loadFavorites();
    void loadFavoritesRecursive(const QJsonArray& array, QStandardItem* parent);
    void saveFavoritesRecursive(QStandardItem* item, QJsonArray& array);
    void buildFavoriteTree(const QJsonArray& array, FavoriteItem* parent);
    void serializeFavoriteTree(FavoriteItem* root, QJsonArray& array);
    FavoriteItem* findFavoriteByUrl(const QUrl& url, FavoriteItem* root = nullptr);

    void createDefaultFolder();
    void populateTree(FavoriteItem* node, QStandardItem* parent);
};

#endif // FAVORITESMANAGER_H
