#ifndef FAVORITEITEM_H
#define FAVORITEITEM_H

#include <QString>
#include <QList>

struct FavoriteItem {
    int id;
    QString title;
    QString url;
    QString iconPath;
    QList<FavoriteItem*> children;
    FavoriteItem* parent;

    FavoriteItem(int id = -1, QString title = "", QString url = "", QString iconPath = "",
                 QList<FavoriteItem*> children = {}, FavoriteItem* parent = nullptr)
        : id(id), title(title), url(url), iconPath(iconPath), children(children), parent(parent) {}
};

#endif // FAVORITEITEM_H
