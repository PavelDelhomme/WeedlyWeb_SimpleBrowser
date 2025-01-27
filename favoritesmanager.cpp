#include "favoritesmanager.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>

FavoritesManager::FavoritesManager(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Gérer les favoris"));
    resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);
    m_favoritesTree = new QTreeWidget(this);
    m_favoritesTree->setHeaderLabels({tr("Nom"), tr("URL"), tr("Tags")});
    layout->addWidget(m_favoritesTree);


    QHBoxLayout *editLayout = new QHBoxLayout;
    m_nameEdit = new QLineEdit(this);
    m_urlEdit = new QLineEdit(this);
    m_tagsEdit = new QLineEdit(this);
    editLayout->addWidget(new QLabel(tr("Nom:")));
    editLayout->addWidget(m_nameEdit);
    editLayout->addWidget(new QLabel(tr("URL:")));
    editLayout->addWidget(m_urlEdit);
    editLayout->addWidget(new QLabel(tr("Tags:")));
    editLayout->addWidget(m_tagsEdit);
    layout->addLayout(editLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addFolderButton = new QPushButton(tr("Ajouter dossier"), this);
    QPushButton *addFavoriteButton = new QPushButton(tr("Ajouter favori"), this);
    QPushButton *deleteButton = new QPushButton(tr("Supprimer"), this);
    QPushButton *editButton = new QPushButton(tr("Modifier"), this);
    buttonLayout->addWidget(addFolderButton);
    buttonLayout->addWidget(addFavoriteButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(editButton);
    layout->addLayout(buttonLayout);

    connect(addFolderButton, &QPushButton::clicked, this, &FavoritesManager::addFolder);
    connect(addFavoriteButton, &QPushButton::clicked, this, &FavoritesManager::addFavorite);
    connect(deleteButton, &QPushButton::clicked, this, &FavoritesManager::deleteFavorite);
    connect(editButton, &QPushButton::clicked, this, &FavoritesManager::editFavorite);

    loadFavorites();
}

void FavoritesManager::loadFavorites()
{
    QFile file("favorites.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        loadFavoritesRecursive(favoritesArray, nullptr);
        file.close();
    }
}



void FavoritesManager::loadFavoritesRecursive(const QJsonArray& array, QTreeWidgetItem* parent)
{
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QTreeWidgetItem* item = addTreeItem(parent, obj["name"].toString(), obj["url"].toString(), obj["tags"].toString());

        if (obj.contains("children")) {
            loadFavoritesRecursive(obj["children"].toArray(), item);
        }
    }
}

QTreeWidgetItem* FavoritesManager::addTreeItem(QTreeWidgetItem* parent, const QString& name, const QString& url, const QString& tags)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent ? parent : m_favoritesTree->invisibleRootItem());
    item->setText(0, name);
    item->setText(1, url);
    item->setText(2, tags);
    return item;
}

void FavoritesManager::addFolder()
{
    QString folderName = QInputDialog::getText(this, tr("Nouveau dossier"), tr("Nom du dossier:"));
    if (!folderName.isEmpty()) {
        addTreeItem(m_favoritesTree->currentItem(), folderName);
        saveFavorites();
    }
}


void FavoritesManager::addFavorite()
{
    if (!m_nameEdit->text().isEmpty() && !m_urlEdit->text().isEmpty()) {
        addTreeItem(m_favoritesTree->currentItem(), m_nameEdit->text(), m_urlEdit->text(), m_tagsEdit->text());
        saveFavorites();
        m_nameEdit->clear();
        m_urlEdit->clear();
        m_tagsEdit->clear();
    }
}

void FavoritesManager::deleteFavorite()
{
    QTreeWidgetItem* currentItem = m_favoritesTree->currentItem();
    if (currentItem) {
        delete currentItem;
        saveFavorites();
    }
}

void FavoritesManager::editFavorite()
{
    QTreeWidgetItem* currentItem = m_favoritesTree->currentItem();
    if (currentItem) {
        currentItem->setText(0, m_nameEdit->text());
        currentItem->setText(1, m_urlEdit->text());
        currentItem->setText(2, m_tagsEdit->text());
        saveFavorites();
    }
}

void FavoritesManager::saveFavorites()
{
    QJsonArray favoritesArray;
    saveFavoritesRecursive(m_favoritesTree->invisibleRootItem(), favoritesArray);

    QFile file("favorites.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(favoritesArray);
        file.write(saveDoc.toJson());
        file.close();
    }
}

void FavoritesManager::saveFavoritesRecursive(QTreeWidgetItem* item, QJsonArray& array)
{
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        QJsonObject obj;
        obj["name"] = child->text(0);
        obj["url"] = child->text(1);
        obj["tags"] = child->text(2);

        if (child->childCount() > 0) {
            QJsonArray childArray;
            saveFavoritesRecursive(child, childArray);
            obj["children"] = childArray;
        }

        array.append(obj);
    }
}
