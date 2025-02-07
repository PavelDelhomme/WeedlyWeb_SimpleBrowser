#include "browserwindow.h"
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
#include <QDropEvent>
#include <QMenu>
#include <QDir>

FavoritesManager::FavoritesManager(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Gérer les favoris"));
    resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);
    m_favoritesTree = new QTreeView(this);
    m_favoritesModel = new QStandardItemModel(this);
    m_favoritesTree->setModel(m_favoritesModel);
    m_favoritesTree->setDragEnabled(true);
    m_favoritesTree->setAcceptDrops(true);
    m_favoritesTree->setDropIndicatorShown(true);
    m_favoritesTree->setDragDropMode(QAbstractItemView::InternalMove);

    m_favoritesModel->setItemPrototype(new QStandardItem);
    m_favoritesModel->setHorizontalHeaderLabels({tr("Nom"), tr("URL"), tr("Tags")});
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

    connect(m_favoritesTree, &QTreeView::customContextMenuRequested, this, &FavoritesManager::showContextMenu);
    m_favoritesTree->setContextMenuPolicy(Qt::CustomContextMenu);

    loadFavorites();
}

void FavoritesManager::loadFavorites()
{
    m_favoritesModel->clear();
    createDefaultFolder();

    QString filePath = QDir::currentPath() + "/src/favorites/favorites.json";
    QFile file(filePath);
    if (!file.exists()) {
        QDir().mkpath(QFileInfo(filePath).path());
        if (file.open(QIODevice::WriteOnly)) {
            file.write("[]");
            file.close();
        }
    }
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray favoritesArray = doc.array();
        loadFavoritesRecursive(favoritesArray, m_favoritesModel->item(0));
        file.close();
    }
}


void FavoritesManager::dropEvent(QDropEvent *event)
{
    QModelIndex dropIndex = m_favoritesTree->indexAt(event->position().toPoint());
    QStandardItem* dropItem = m_favoritesModel->itemFromIndex(dropIndex);
    QList<QStandardItem*> draggedItems;

    for (const QModelIndex &index : m_favoritesTree->selectionModel()->selectedIndexes()) {
        if (index.column() == 0) {
            draggedItems << m_favoritesModel->itemFromIndex(index)->clone();
        }
    }

    for (QStandardItem* item : draggedItems) {
        if (dropItem && dropItem->data(Qt::UserRole) == "folder") {
            dropItem->appendRow(item);
        } else {
            m_favoritesModel->appendRow(item);
        }
    }

    for (const QModelIndex &index : m_favoritesTree->selectionModel()->selectedIndexes()) {
        if (index.column() == 0) {
            m_favoritesModel->removeRow(index.row(), index.parent());
        }
    }
    saveFavorites();
    event->acceptProposedAction();
}


void FavoritesManager::loadFavoritesRecursive(const QJsonArray& array, QStandardItem* parent)
{
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QList<QStandardItem*> items;
        items << new QStandardItem(obj["name"].toString());
        items << new QStandardItem(obj["url"].toString());
        items << new QStandardItem(obj["tags"].toString());
        parent->appendRow(items);

        if (obj.contains("children")) {
            loadFavoritesRecursive(obj["children"].toArray(), items[0]);
        }
    }
}

void FavoritesManager::addFolder()
{
    QString folderName = QInputDialog::getText(this, tr("Nouveau dossier"), tr("Nom du dossier:"));
    if (!folderName.isEmpty()) {
        QStandardItem *folder = new QStandardItem(folderName);
        folder->setData("folder", Qt::UserRole);
        QModelIndex index = m_favoritesTree->currentIndex();
        if (index.isValid()) {
            m_favoritesModel->itemFromIndex(index)->appendRow(folder);
        } else {
            m_favoritesModel->appendRow(folder);
        }
        saveFavorites();
    }
}

void FavoritesManager::addFavorite()
{
    if (!m_nameEdit->text().isEmpty() && !m_urlEdit->text().isEmpty()) {
        QList<QStandardItem*> items;
        items << new QStandardItem(m_nameEdit->text());
        items << new QStandardItem(m_urlEdit->text());
        items << new QStandardItem(m_tagsEdit->text());
        QModelIndex index = m_favoritesTree->currentIndex();
        if (index.isValid()) {
            m_favoritesModel->itemFromIndex(index)->appendRow(items);
        } else {
            m_favoritesModel->appendRow(items);
        }
        saveFavorites();
        m_nameEdit->clear();
        m_urlEdit->clear();
        m_tagsEdit->clear();
    }
}

void FavoritesManager::deleteFavorite()
{
    QModelIndex index = m_favoritesTree->currentIndex();
    if (index.isValid()) {
        m_favoritesModel->removeRow(index.row(), index.parent());
        saveFavorites();
    }
}

void FavoritesManager::editFavorite()
{
    QModelIndex index = m_favoritesTree->currentIndex();
    if (index.isValid()) {
        m_favoritesModel->setData(index, m_nameEdit->text());
        m_favoritesModel->setData(index.sibling(index.row(), 1), m_urlEdit->text());
        m_favoritesModel->setData(index.sibling(index.row(), 2), m_tagsEdit->text());
        saveFavorites();
    }
}
void FavoritesManager::saveFavorites()
{
    QJsonArray favoritesArray;
    saveFavoritesRecursive(m_favoritesModel->invisibleRootItem(), favoritesArray);

    QFile file("src/favorites/favorites.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(favoritesArray);
        file.write(saveDoc.toJson());
        file.close();
    } else {
        qWarning() << "Impossible d'ouvrir le fichier favorites.json pour l'écriture";
    }
}

void FavoritesManager::saveFavoritesRecursive(QStandardItem* item, QJsonArray& array)
{
    for (int i = 0; i < item->rowCount(); ++i) {
        QStandardItem* child = item->child(i);
        QJsonObject obj;
        obj["title"] = child->text();

        bool isFolder = child->data(Qt::UserRole).toString() == "folder";
        obj["folder"] = isFolder;

        if (!isFolder) {
            obj["url"] = child->data(Qt::UserRole + 1).toString();
            obj["favicon"] = child->data(Qt::UserRole + 2).toString();
        }

        if (child->hasChildren()) {
            QJsonArray childArray;
            saveFavoritesRecursive(child, childArray);
            obj["children"] = childArray;
        }

        array.append(obj);
    }
}


void FavoritesManager::createDefaultFolder()
{
    QStandardItem* defaultFolder = new QStandardItem(tr("Favoris"));
    defaultFolder->setData("folder", Qt::UserRole);
    m_favoritesModel->appendRow(defaultFolder);
}


void FavoritesManager::showContextMenu(const QPoint &pos)
{
    QModelIndex index = m_favoritesTree->indexAt(pos);
    QMenu contextMenu(this);

    QAction *addFolderAction = contextMenu.addAction(tr("Ajouter un dossier"));
    QAction *addFavoriteAction = contextMenu.addAction(tr("Ajouter un favori"));
    QAction *deleteAction = contextMenu.addAction(tr("Supprimer"));

    connect(addFolderAction, &QAction::triggered, this, &FavoritesManager::addFolder);
    connect(addFavoriteAction, &QAction::triggered, this, &FavoritesManager::addFavorite);
    connect(deleteAction, &QAction::triggered, this, &FavoritesManager::deleteFavorite);

    contextMenu.exec(m_favoritesTree->viewport()->mapToGlobal(pos));
}



void FavoritesManager::buildFavoriteTree(const QJsonArray& array, FavoriteItem* parent)
{
    // Remplacer par une version SQLite
    QVector<QMap<QString, QVariant>> favorites = m_database.getFavorites(parent->id);
    for (const auto &fav : favorites) {
        FavoriteItem *item = new FavoriteItem(
            fav["id"].toInt(),
            fav["title"].toString(),
            fav["url"].toString(),
            fav["icon_path"].toString(),
            {},
            parent
        );
        parent->children.append(item);
        buildFavoriteTree(QJsonArray(), item); // Appel récursif pour les enfants
    }
}

void FavoritesManager::serializeFavoriteTree(FavoriteItem* root, QJsonArray& array)
{
    BrowserWindow* browserWindow = qobject_cast<BrowserWindow*>(parent());
    if (browserWindow) {
        browserWindow->ensureFavoritesFileExists();
    }
    for (FavoriteItem* item : root->children) {
        QJsonObject obj;
        obj["title"] = item->title;
        obj["url"] = item->url;
        obj["iconPath"] = item->iconPath;

        if (!item->children.isEmpty()) {
            QJsonArray childArray;
            serializeFavoriteTree(item, childArray);
            obj["children"] = childArray;
        }

        array.append(obj);
    }
}

FavoriteItem* FavoritesManager::findFavoriteByUrl(const QUrl& url, FavoriteItem* root)
{
    if (!root) {
        root = m_favoritesRoot;
    }

    if (root->url == url.toString()) {
        return root;
    }

    for (FavoriteItem* child : root->children) {
        FavoriteItem* found = findFavoriteByUrl(url, child);
        if (found) {
            return found;
        }
    }

    return nullptr;
}

void FavoritesManager::updateTreeView(FavoriteItem* root)
{
    m_favoritesRoot = root;
    m_favoritesModel->clear();
    createDefaultFolder();
    populateTree(root, m_favoritesModel->invisibleRootItem());
}

