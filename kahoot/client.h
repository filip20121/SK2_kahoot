#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QTextStream>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

    struct Quiz
    {
        int AccessCode;
        QString questionTxt[10];
        QString answer[10];
        int capacity;
    }quiz1;

void sendBtnHit();
void dataReceived();

protected:
    /* TODO: Dodać zmienną reprezentująca gniazdo - wskaźnik na  QTcpSocket */
    QTcpSocket *sock;
    QTimer * connTimeoutTimer;
    QTextStream os(QTcpSocket sock);

    void connectBtnHit();

    void socketConnected();
    void socketDisconnected();
    void socketError(QTcpSocket::SocketError);
    void createQuiz();
    void addQuestion();
    void joinToTheGame();
    void undoSetDisable();
    void startTheGame();
    /* TODO: Stworzyć nagłówki pozostałych funkcji obsługujących zdarzenie na gnieździe */

private:
    Ui::Client *ui;


signals:
    void resultReady(const QString &result);
};

class Sender : public QObject
{
    Q_OBJECT
    QThread sender;

 public:

 //protected:
   Sender() {
        Client *senderClient = new Client;
                senderClient->moveToThread(&sender);
                connect(&sender, &QThread::finished, senderClient, &QObject::deleteLater);
                connect(this, &Sender::operate, senderClient, &Client::sendBtnHit);
      //          connect(senderClient, &Client::resultReady, this, &Sender::handleResults);
                sender.start();
    }

   ~Sender() {
           sender.quit();
           sender.wait();
       }
public slots:
    //void handleResults(const QString &);

signals:
    void operate(const QString &);
};


class Listener : public QObject
{
    Q_OBJECT
    QThread listen;

 public:

 //protected:
   Listener() {
        Client *listener = new Client;
                listener->moveToThread(&listen);
                connect(&listen, &QThread::finished, listener, &QObject::deleteLater);
                connect(this, &Listener::operate, listener, &Client::dataReceived);
          //      connect(listener, &Client::resultReady, this, &Listener::handleResults);
                listen.start();
    }

   ~Listener() {
           listen.quit();
           listen.wait();
       }
public slots:
    //void handleResults(const QString &);

signals:
    void operate(const QString &);
};

