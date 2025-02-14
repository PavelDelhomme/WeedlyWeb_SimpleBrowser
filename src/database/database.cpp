#include "database.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>


Database::Database(QObject *parent) : QObject(parent) {}

bool Database::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open())
    {
        qWarning() << "Impossible d'ouvrir la base de données : " << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS folders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "parent_id INTEGER DEFAULT 0)");


    query.exec("CREATE TABLE IF NOT EXISTS favorites ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "title TEXT NOT NULL, "
               "url TEXT NOT NULL, "
               "icon_path TEXT, "
               "parent_id INTEGER DEFAULT 0, "
               "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // Vérifier si le dossier "Favoris" existe déjà
    query.prepare("SELECT id FROM folders WHERE name = 'Favoris'");
    query.exec();
    int favoritesFolderId = 0; // Initialiser à une valeur par défaut
    
    if (query.next()) {
        favoritesFolderId = query.value(0).toInt();
    } else {
        // Créer le dossier "Favoris" s'il n'existe pas
        query.prepare("INSERT INTO folders (name, parent_id) VALUES ('Favoris', 0)");
        if (query.exec()) {
            favoritesFolderId = query.lastInsertId().toInt(); // Récupérer l'ID
        } else {
            qWarning() << "Erreur lors de l'insertion du dossier Favoris:" << query.lastError().text();
            return false;
        }
    }

    QSqlQuery("CREATE INDEX IF NOT EXISTS idx_parent_id ON favorites(parent_id)");
    QSqlQuery("CREATE INDEX IF NOT EXISTS idx_url ON favorites(url)");

    return true;
}

bool Database::addFavorite(const QString &title, const QString &url, const QString &iconPath, int parentId)
{
    QSqlQuery query;
    query.prepare("INSERT INTO favorites (title, url, icon_path, parent_id) "
                  "VALUES (:title, :url, :icon_path, :parent_id)");
    query.bindValue(":title", title);
    query.bindValue(":url", url);
    query.bindValue(":icon_path", iconPath);
    query.bindValue(":parent_id", parentId);

    return query.exec();
}


bool Database::deleteFavorite(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM favorites WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QVector<QMap<QString, QVariant>> Database::getFavorites(int parentId)

{
    QVector<QMap<QString, QVariant>> results;
    QSqlQuery query;
    
    if(parentId == 0) {
        query.prepare("SELECT id, title, url, icon_path, parent_id FROM favorites");
    } else {
        query.prepare("SELECT id, title, url, icon_path, parent_id FROM favorites WHERE parent_id = :parent_id");
        query.bindValue(":parent_id", parentId);
    }
    
    if(query.exec()) {
        while(query.next()) {
            QMap<QString, QVariant> row;
            row["id"] = query.value(0);
            row["title"] = query.value(1);
            row["url"] = query.value(2);
            row["icon_path"] = query.value(3);
            row["parent_id"] = query.value(4);
            results.append(row);
        }
    }
    return results;
}

bool Database::updateFavicon(int id, const QString& faviconPath)
{
    QSqlQuery query;
    query.prepare("UPDATE favorites SET icon_path = :path WHERE id = :id");
    query.bindValue(":path", faviconPath);
    query.bindValue(":id", id);
    return query.exec();
}



QMap<QString, QVariant> Database::getFavoriteByUrl(const QUrl& url) const
{
    QSqlQuery query;
    query.prepare("SELECT id, title, icon_path, parent_id FROM favorites WHERE url = :url");
    query.bindValue(":url", url.toString());
    
    if(query.exec() && query.next()) {
        return {
            {"id", query.value(0)},
            {"title", query.value(1)},
            {"icon_path", query.value(2)},
            {"parent_id", query.value(3)}
        };
    }
    return {};
}


bool Database::updateFavorite(int id, const QString &newTitle, const QString &newUrl, int parentId)
{
    QSqlQuery query;
    query.prepare("UPDATE favorites SET title = ?, url = ?, parent_id = ? WHERE id = ?");
    query.addBindValue(newTitle);
    query.addBindValue(newUrl);
    query.addBindValue(parentId);
    query.addBindValue(id);
    return query.exec();
}


bool Database::migrateFromJson()
{
    QFile file("src/favorites/favorites.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossible d'ouvrir le fichier JSON des favoris";
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        qWarning() << "Le fichier JSON des favoris n'est pas un tableau";
        return false;
    }

    QJsonArray favoritesArray = doc.array();

    m_db.transaction();

    for (const QJsonValue &value : favoritesArray) {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        QString url = obj["url"].toString();
        QString iconPath = obj["favicon"].toString();
        int parentId = obj["folder"].toBool() ? 0 : obj["parent_id"].toInt(0);

        if (!addFavorite(title, url, iconPath, parentId)) {
            m_db.rollback();
            qWarning() << "Échec de la migration : impossible d'ajouter le favori" << title;
            return false;
        }

        if (obj.contains("children")) {
            QJsonArray children = obj["children"].toArray();
            for (const QJsonValue &childValue : children) {
                QJsonObject childObj = childValue.toObject();
                QString childTitle = childObj["title"].toString();
                QString childUrl = childObj["url"].toString();
                QString childIconPath = childObj["favicon"].toString();

                if (!addFavorite(childTitle, childUrl, childIconPath, parentId)) {
                    m_db.rollback();
                    qWarning() << "Échec de la migration : impossible d'ajouter le favori enfant" << childTitle;
                    return false;
                }
            }
        }
    }

    if (!m_db.commit()) {
        qWarning() << "Échec de la migration : impossible de valider la transaction";
        return false;
    }

    return true;
}