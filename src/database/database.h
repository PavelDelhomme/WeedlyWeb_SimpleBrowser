#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    bool initDatabase();
    bool migrateFromJson();

    // Opération CRUD
    bool addFavorite(const QString &title, const QString &url, const QString &iconPath, int parentId = 0);
    bool deleteFavorite(int id);
    bool updateFavorite(int id, const QString &title, const QString &url, int parentId);
    QVector<QMap<QString, QVariant>> getFavorites(int parentId = 0);

private:
    QSqlDatabase m_db;
    QString m_dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/favorites.db";
    QVector<QMap<QString, QVariant>> Database::getFavorites();
};

#endif // DATABASE_H