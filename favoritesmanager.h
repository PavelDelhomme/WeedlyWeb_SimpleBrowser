#ifndef FAVORITESMANAGER_H
#define FAVORITESMANAGER_H

#include <QDialog>
#include <QTreeView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QDropEvent>
#include <QMenu>

class FavoritesManager : public QDialog
{
    Q_OBJECT;

public:
    explicit FavoritesManager(QWidget *parent = nullptr);
    void showContextMenu(const QPoint &pos);

protected:
    void dropEvent(QDropEvent *event) override;

private slots:
    void addFolder();
    void addFavorite();
    void deleteFavorite();
    void editFavorite();
    void saveFavorites();
    void createDefaultFolder();

private:
    QTreeView *m_favoritesTree;
    QStandardItemModel *m_favoritesModel;
    void loadFavorites();
    void loadFavoritesRecursive(const QJsonArray& array, QStandardItem* parent);
    void saveFavoritesRecursive(QStandardItem* item, QJsonArray& array);
    QLineEdit *m_nameEdit;
    QLineEdit *m_urlEdit;
    QLineEdit *m_tagsEdit;
};

#endif // FAVORITESMANAGER_H
