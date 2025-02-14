#include "commandpalette.h"
#include "webview.h"
#include "requestinterceptor.h"

#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>
#include <QComboBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QListWidget>
#include <QTextEdit>
#include <QShortcut>
#include <QTableWidget>
#include <QPainter>
#include <QCompleter>
#include <QFile>
#include <QStringListModel>
#include <QDrag>
#include <QMimeData>
#include <QFormLayout>
#include <QBuffer>
#include <QTimer>
#include <QHeaderView>
#include <QPushButton>
#include <QNetworkRequest>
#include <QUrl>


CommandPalette::CommandPalette(QWidget *parent) : QWidget(parent), m_currentWebView(nullptr), m_requestInterceptor(nullptr) {
    setWindowFlags(Qt::Popup);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_lineEdit = new QLineEdit(this);
    m_listWidget = new QListWidget(this);
    
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_listWidget);

    m_commands << "/cvec detect" << "/request GET" << "/request POST" << "/analyze";
    setupCompleter();
    
    connect(m_lineEdit, &QLineEdit::textChanged, this, &CommandPalette::filterCommands);
    connect(m_listWidget, &QListWidget::itemActivated, this, &CommandPalette::onCommandSelected);
}


void CommandPalette::setupCompleter()
{
    m_completer = new QCompleter(m_commands, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchStartsWith);
    m_lineEdit->setCompleter(m_completer);
}

void CommandPalette::addCommand(const QString &command)
{
    if (!m_commands.contains(command)) {
        m_commands << command;
        m_completer->setModel(new QStringListModel(m_commands, m_completer));
    }
}

void CommandPalette::setCurrentWebView(WebView *webView) {
    m_currentWebView = webView;
}

void CommandPalette::setRequestInterceptor(RequestInterceptor *interceptor) {
    m_requestInterceptor = interceptor;
}

void CommandPalette::filterCommands(const QString &text) {
    m_listWidget->clear();
    QStringList commands = {"/analyze", "/request GET", "/request POST", "/cvec detect"};
    
    for (const QString &cmd : commands) {
        if (cmd.startsWith(text, Qt::CaseInsensitive)) {
            m_listWidget->addItem(cmd);
        }
    }
}

void CommandPalette::showPalette()
{
    m_lineEdit->clear();
    filterCommands("");
    show();
    m_lineEdit->setFocus();
    activateWindow(); // Make window activated
}

void CommandPalette::onCommandSelected(QListWidgetItem *item)
{
    if (item) {
        processCommand(item->text());
        hide();
    }
}


void CommandPalette::processCommand(const QString &command)
{
    if (command.startsWith("/cvec")) {
        processCVECommand(command);
    } else if (command.startsWith("/request")) {
        processRequestCommand(command);
    } else if (command.startsWith("/analyze")) {
        showRequestAnalyzer();
    } else {
        qDebug() << "Commande inconnue : " << command;
    }
}


void CommandPalette::processCVECommand(const QString &command) {
    if (command == "/cvec detect") {
        if (m_currentWebView) {
            m_currentWebView->page()->toHtml([this](const QString &result) {
                QStringList detectedCVEs = detectCVEs(result);
                displayCVEResults(detectedCVEs);
            });
        }
    }
}

void CommandPalette::processRequestCommand(const QString &command) {
    QStringList parts = command.split(" ");
    if (parts.size() >= 2) {
        QString method = parts[1].toUpper(); // Convertir en majuscules pour éviter les erreurs
        QString url = m_currentWebView->url().toString();

        if (method == "GET") {
            sendGetRequest(url);
        } else if (method == "POST") {
            // Demander à l'utilisateur de fournir les données POST
            bool ok;
            QString postData = QInputDialog::getText(this, tr("Données POST"),
                                                     tr("Entrez les données POST :"), QLineEdit::Normal,
                                                     "", &ok);
            if (ok && !postData.isEmpty()) {
                sendPostRequest(url, postData);
            }
        } else if (method == "ANALYZE") {
            showRequestAnalyzer();
        } else {
            qDebug() << "Méthode de requête inconnue : " << method;
        }
    }
}



QStringList CommandPalette::detectCVEs(const QString &html) {
    QStringList cves;
    QRegularExpression re("CVE-\\d{4}-\\d{4,7}");
    QRegularExpressionMatchIterator i = re.globalMatch(html);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        cves << match.captured(0);
    }
    return cves;
}


void CommandPalette::displayCVEResults(const QStringList &cves) {
    QDialog *resultDialog = new QDialog(this);
    resultDialog->setWindowTitle(tr("Résultats de détection CVE"));
    QVBoxLayout *layout = new QVBoxLayout(resultDialog);

    QListWidget *listWidget = new QListWidget(resultDialog);
    listWidget->addItems(cves);
    layout->addWidget(listWidget);

    resultDialog->setLayout(layout);
    resultDialog->show();
}

void CommandPalette::sendPostRequest(const QString &url, const QString &data) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &CommandPalette::handleNetworkReply);

    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray postData;
    postData.append(data.toUtf8()); // Convertir le QString en QByteArray
    
    manager->post(request, postData);
}

void CommandPalette::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Erreur : " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    QDialog *resultDialog = new QDialog(this);
    resultDialog->setWindowTitle("Résultat de la requête");
    QVBoxLayout *layout = new QVBoxLayout(resultDialog);

    QTextEdit *textEdit = new QTextEdit(resultDialog);
    textEdit->setPlainText(answer);
    layout->addWidget(textEdit);

    resultDialog->setLayout(layout);
    resultDialog->show();

    reply->deleteLater();
}

void CommandPalette::sendGetRequest(const QString &url) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &CommandPalette::handleNetworkReply);
    manager->get(QNetworkRequest(QUrl(url)));
}
void CommandPalette::showRequestAnalyzer() {
    QDialog *analyzerDialog = new QDialog(this);
    analyzerDialog->setWindowTitle(tr("Analyseur de requêtes"));
    QVBoxLayout *layout = new QVBoxLayout(analyzerDialog);

    QTableWidget *requestTable = new QTableWidget(analyzerDialog);
    requestTable->setColumnCount(3);
    requestTable->setHorizontalHeaderLabels({tr("ID"), tr("URL"), tr("Méthode")});
    requestTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (m_requestInterceptor) {
        auto requests = m_requestInterceptor->getInterceptedRequests();
        requestTable->setRowCount(requests.size());

        int row = 0;
        for (auto it = requests.begin(); it != requests.end(); ++it, ++row) {
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(it.key().toInt()));
            QTableWidgetItem *urlItem = new QTableWidgetItem(it.value().url().toString());
            QTableWidgetItem *methodItem = new QTableWidgetItem(it.value().attribute(QNetworkRequest::CustomVerbAttribute).toString());
            
            requestTable->setItem(row, 0, idItem);
            requestTable->setItem(row, 1, urlItem);
            requestTable->setItem(row, 2, methodItem);
        }
    }

    layout->addWidget(requestTable);

    QPushButton *closeButton = new QPushButton(tr("Fermer"), analyzerDialog);
    connect(closeButton, &QPushButton::clicked, analyzerDialog, &QDialog::accept);
    layout->addWidget(closeButton);

    analyzerDialog->setLayout(layout);
    analyzerDialog->exec();
}
