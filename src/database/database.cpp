#include "database.h"
#include <QDir>

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
                "parent_id INTEGER DEFAULT 0");
    

    query.exec("CREATE TABLE IF NOT EXISTS favorites ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "title TEXT NOT NULL, "
               "url TEXT NOT NULL, "
               "icon_path TEXT, "
               "parent_id INTEGER DEFAULT 0, "
               "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

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



QVector<QMap<QString, QVariant>> Database::getFavorites()
{
    QVector<QMap<QString, QVariant>> results;
    QSqlQuery query("SELECT id, title, url, icon_path, parent_id FROM favorites");
    
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
