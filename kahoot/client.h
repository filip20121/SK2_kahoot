#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QTextStream>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class CLient; }
QT_END_NAMESPACE

class CLient : public QWidget
{
    Q_OBJECT

public:
    explicit CLient(QWidget *parent = 0);
    ~CLient();

    struct Quiz
    {
        QString AccessCode;
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
    void startUp();
    /* TODO: Stworzyć nagłówki pozostałych funkcji obsługujących zdarzenie na gnieździe */

private:
    Ui::CLient *ui;


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
        CLient *senderCLient = new CLient;
                senderCLient->moveToThread(&sender);
                connect(&sender, &QThread::finished, senderCLient, &QObject::deleteLater);
                connect(this, &Sender::operate, senderCLient, &CLient::sendBtnHit);
      //          connect(senderCLient, &CLient::resultReady, this, &Sender::handleResults);
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
        CLient *listener = new CLient;
                listener->moveToThread(&listen);
                connect(&listen, &QThread::finished, listener, &QObject::deleteLater);
                connect(this, &Listener::operate, listener, &CLient::dataReceived);
          //      connect(listener, &CLient::resultReady, this, &Listener::handleResults);
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

