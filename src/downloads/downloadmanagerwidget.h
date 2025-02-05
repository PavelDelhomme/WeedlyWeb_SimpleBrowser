#ifndef DOWNLOADMANAGERWIDGET_H
#define DOWNLOADMANAGERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE

class DownloadWidget;

class DownloadManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadManagerWidget(QWidget *parent = nullptr);
    void downloadRequested(QWebEngineDownloadRequest *webItem);

private:
    void add(DownloadWidget *downloadWidget);
    void remove(DownloadWidget *downloadWidget);

    QVBoxLayout *m_topLevelLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_items;
    QVBoxLayout *m_itemsLayout;
    QLabel *m_zeroItemsLabel;
    int m_numDownloads = 0;
};

#endif // DOWNLOADMANAGERWIDGET_H
