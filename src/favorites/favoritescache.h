#include "database.h"
#include "browserwindow.h"

class FavoritesCache {
public:
    static FavoritesCache& instance() {
        static FavoritesCache instance;
        return instance;
    }

    void invalidate() { 
        delete m_root;
        m_root = nullptr;
        m_valid = false;
    }

    FavoriteItem* getRoot(Database& db) {
        if(!m_valid || !m_root) {
            loadFromDatabase(db);
            m_valid = true;
        }
        return m_root;
    }

private:
    void loadFromDatabase(Database& db) {
        QVector<QMap<QString, QVariant>> allFavorites = db.getFavorites();
        QHash<int, FavoriteItem*> items;

        // Création des items
        for(const auto& fav : allFavorites) {
            items[fav["id"].toInt()] = new FavoriteItem{
                fav["id"].toInt(),
                fav["title"].toString(),
                fav["url"].toString(),
                fav["icon_path"].toString(),
                {},
                nullptr
            };
        }

        // Construction hiérarchique
        for(const auto& fav : allFavorites) {
            int parentId = fav["parent_id"].toInt();
            if(parentId > 0 && items.contains(parentId)) {
                items[parentId]->children.append(items[fav["id"].toInt()]);
                items[fav["id"].toInt()]->parent = items[parentId];
            }
        }

        m_root = items.value(0, new FavoriteItem{-1, "Root", "", "", {}, nullptr});
    }

    bool m_valid = false;
    FavoriteItem* m_root = nullptr;
};
