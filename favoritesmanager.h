#ifndef FAVORITESMANAGER_H
#define FAVORITESMANAGER_H

#include <QDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QLineEdit>

class FavoritesManager : public QDialog
{
    Q_OBJECT

public:
    explicit FavoritesManager(QWidget *parent = nullptr);

private slots:
    void addFolder();
    void addFavorite();
    void deleteFavorite();
    void editFavorite();
    void saveFavorites();

private:
    void loadFavorites();
    QTreeWidgetItem* addTreeItem(QTreeWidgetItem* parent, const QString& name, const QString& url = QString(), const QString& tags = QString());
    void loadFavoritesRecursive(const QJsonArray& array, QTreeWidgetItem* parent);
    void saveFavoritesRecursive(QTreeWidgetItem* item, QJsonArray& array);
    QTreeWidget *m_favoritesTree;
    QLineEdit *m_nameEdit;
    QLineEdit *m_urlEdit;
    QLineEdit *m_tagsEdit;
};

#endif // FAVORITESMANAGER_H
