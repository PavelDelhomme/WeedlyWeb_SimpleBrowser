#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <qelapsedtimer.h>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE

class DownloadWidget : public QFrame
{
    Q_OBJECT
public:
    explicit DownloadWidget(QWebEngineDownloadRequest *download, QWidget *parent = nullptr);

signals:
    void removeClicked(DownloadWidget *self);

private slots:
    void updateWidget();

private:
    QString withUnit(qreal bytes);
    
    QGridLayout *m_topLevelLayout;
    QLabel *m_dstName;
    QPushButton *m_cancelButton;
    QLabel *m_srcUrl;
    QProgressBar *m_progressBar;

    QWebEngineDownloadRequest *m_download;
    QElapsedTimer m_timeAdded;
};

#endif // DOWNLOADWIDGET_H
