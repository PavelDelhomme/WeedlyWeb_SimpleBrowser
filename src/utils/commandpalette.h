#ifndef COMMANDPALETTE_H
#define COMMANDPALETTE_H

#include <QDialog>
#include <QTreeView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QDropEvent>
#include <QMenu>
#include <QWidget>
#include <QListWidget>
#include <QNetworkReply>
#include <QCompleter>
#include <QStringList>

class WebView;
class RequestInterceptor;

class CommandPalette : public QWidget {
    Q_OBJECT
public:
    explicit CommandPalette(QWidget *parent = nullptr);
    void showPalette();
    void setCurrentWebView(WebView *webView);
    void setRequestInterceptor(RequestInterceptor *interceptor);
    void processCommand(const QString &command);

signals:
    void commandSelected(const QString &command);

private slots:
    void onCommandSelected(QListWidgetItem *item);

private:
    QLineEdit *m_lineEdit;
    QListWidget *m_listWidget;
    WebView *m_currentWebView;
    RequestInterceptor *m_requestInterceptor;
    QCompleter *m_completer;
    QStringList m_commands;

    void filterCommands(const QString &text);
    void processCVECommand(const QString &command);
    void processRequestCommand(const QString &command);
    QStringList detectCVEs(const QString &html);
    void displayCVEResults(const QStringList &cves);
    void showRequestAnalyzer();
    void sendGetRequest(const QString &url);
    void sendPostRequest(const QString &url, const QString &data);
    void handleNetworkReply(QNetworkReply *reply);
    void setupCompleter();
    void addCommand(const QString &command);
};

#endif // COMMANDPALETTE_H