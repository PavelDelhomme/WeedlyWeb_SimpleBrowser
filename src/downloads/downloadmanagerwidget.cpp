#include "downloadmanagerwidget.h"
#include "downloadwidget.h"

#include <QFileDialog>
#include <QDir>
#include <QWebEngineDownloadRequest>

DownloadManagerWidget::DownloadManagerWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("DownloadManagerWidget");
    resize(400, 212);
    setStyleSheet("#DownloadManagerWidget { background: palette(button)}");

    m_topLevelLayout = new QVBoxLayout(this);
    m_topLevelLayout->setObjectName("m_topLevelLayout");
    m_topLevelLayout->setSizeConstraint(QLayout::SetNoConstraint);
    m_topLevelLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName("m_scrollArea");
    m_scrollArea->setStyleSheet("#m_scrollArea { margin: 2px; border: none; }");
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_items = new QWidget();
    m_items->setObjectName("m_items");
    m_items->setStyleSheet("#m_items { background: palette(mid) }");

    m_itemsLayout = new QVBoxLayout(m_items);
    m_itemsLayout->setSpacing(2);
    m_itemsLayout->setContentsMargins(3, 3, 3, 3);

    m_zeroItemsLabel = new QLabel(m_items);
    m_zeroItemsLabel->setObjectName("m_zeroItemsLabel");
    m_zeroItemsLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_zeroItemsLabel->setStyleSheet("color: palette(shadow)");
    m_zeroItemsLabel->setAlignment(Qt::AlignCenter);
    m_zeroItemsLabel->setText(tr("No downloads"));

    m_itemsLayout->addWidget(m_zeroItemsLabel);
    m_scrollArea->setWidget(m_items);
    m_topLevelLayout->addWidget(m_scrollArea);

    setWindowTitle(tr("Downloads"));
}

void DownloadManagerWidget::downloadRequested(QWebEngineDownloadRequest *download)
{
    Q_ASSERT(download && download->state() == QWebEngineDownloadRequest::DownloadRequested);

    QString path = QFileDialog::getSaveFileName(this, tr("Save as"), QDir(download->downloadDirectory()).filePath(download->downloadFileName()));
    if (path.isEmpty())
        return;

    download->setDownloadDirectory(QFileInfo(path).path());
    download->setDownloadFileName(QFileInfo(path).fileName());
    download->accept();
    add(new DownloadWidget(download));

    show();
}

void DownloadManagerWidget::add(DownloadWidget *downloadWidget)
{
    connect(downloadWidget, &DownloadWidget::removeClicked, this, &DownloadManagerWidget::remove);
    m_itemsLayout->insertWidget(0, downloadWidget, 0, Qt::AlignTop);
    if (m_numDownloads++ == 0)
        m_zeroItemsLabel->hide();
}

void DownloadManagerWidget::remove(DownloadWidget *downloadWidget)
{
    m_itemsLayout->removeWidget(downloadWidget);
    downloadWidget->deleteLater();
    if (--m_numDownloads == 0)
        m_zeroItemsLabel->show();
}
