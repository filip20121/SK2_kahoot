#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QTextStream>

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
        int AccessCode;
        QString questionTxt[10];
        QString answer[10];
    };

protected:
    /* TODO: Dodać zmienną reprezentująca gniazdo - wskaźnik na  QTcpSocket */
    QTcpSocket *sock;
    QTimer * connTimeoutTimer;
    Quiz quiz1;
    QTextStream os(QTcpSocket sock);
    //QByteArray *ba;
    void connectBtnHit();
    void sendBtnHit();
    void dataReceived();
    void socketConnected();
    void createQuiz();
    void addQuestion();
    void startUp();
    /* TODO: Stworzyć nagłówki pozostałych funkcji obsługujących zdarzenie na gnieździe */

private slots:
    void on_port_valueChanged(int arg1);

private:
    Ui::CLient *ui;
};

