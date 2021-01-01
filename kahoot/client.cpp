#include "client.h"
#include "ui_client.h"

#include <QMessageBox>
#include <QTcpServer>
#include <qtextstream.h>
#include <QString>
#include <QMutex>

QMutex mutex;

CLient::CLient(QWidget *parent): QWidget(parent), ui(new Ui::CLient){
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &CLient::connectBtnHit);
    connect(ui->server, &QLineEdit::returnPressed, ui->connectButton, &QPushButton::click);
    connect(ui->send, &QPushButton::clicked, this, &CLient::sendBtnHit);
    connect(ui->create, &QPushButton::clicked, this, &CLient::createQuiz);
    connect(ui->add, &QPushButton::clicked, this, &CLient::addQuestion);
    connect(ui->exitButton, &QPushButton::clicked, this, &CLient::startUp);
    connect(ui->joinButton, &QPushButton::clicked, this, &CLient::joinToTheGame);
}

CLient::~CLient(){
    sock -> close();
    delete ui;
}
void CLient::startUp(){

    ui->connectMenu->setEnabled(true);
    ui->join_game->setEnabled(true);
    ui->connectCreate->setEnabled(true);
    ui->connectQuestion->setEnabled(true);
    ui->quiz->clear();

}

void CLient::connectBtnHit(){
    ui->quiz->clear();
    ui->connect->setEnabled(false);
    ui->connectQuestion->setEnabled(false);
    ui->connectAnswer->setEnabled(false);
    ui->exitButton->setEnabled(false);
    ui->add->setEnabled(false);

    ui->quiz->append("<b>Connecting to " + ui->server->text() + ":" + ui->port->value() + "</b>");
    sock = new QTcpSocket(this);
        connTimeoutTimer = new QTimer(this);
        connTimeoutTimer->setSingleShot(true);

    connect(connTimeoutTimer, &QTimer::timeout, [&]{
            sock->abort();
            sock->deleteLater();
            connTimeoutTimer->deleteLater();
            ui->join_game->setEnabled(true);
            ui->quiz->append("<b>Connect timed out</b>");
            QMessageBox::critical(this, "Error", "Connect timed out");
        });

    connect(sock, &QTcpSocket::connected, this, &CLient::socketConnected);
    connect(sock, &QTcpSocket::disconnected, this, &CLient::socketDisconnected);
    connect(sock, (void(QTcpSocket::*)(QTcpSocket::SocketError)) &QTcpSocket::error, this, &CLient::socketError);
    connect(sock, &QTcpSocket::readyRead, this, &CLient::dataReceived);

    sock -> connectToHost(ui->server->text(), ui->port->value());
    connTimeoutTimer->start(3000);
}

void CLient::socketConnected(){
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    ui->quiz->append("<b>Connected</b>");

}

void CLient::socketDisconnected(){
    ui->quiz->append("<b>Disconnected</b>");
    ui->connect->setEnabled(true);
    startUp();
}

void CLient::socketError(QTcpSocket::SocketError err){
    if(err == QTcpSocket::RemoteHostClosedError)
        return;
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->quiz->append("<b>Socket error: "+sock->errorString()+"</b>");
    ui->connect->setEnabled(true);
    startUp();
}

/*
 * function dataReceived()
 *
 * is responsible for reading data from the server
 * data is like question with posssible answers,
 *              your score,
*/
void CLient::dataReceived(){
    QByteArray ba = sock->readAll();

     if(QString::fromUtf8(ba)[0] == "!"){
        //if quiz ended
        if(QString::fromUtf8(ba)[1] == "#")
        {
            ui->exitButton->setEnabled(true);
            ui->quiz->append(QString::fromUtf8(ba).trimmed());
            ui->quiz->setAlignment(Qt::AlignLeft);

        }
        //if code is incorrect
        if(QString::fromUtf8(ba)[1] == "*")
        {
            ui->join_game->setEnabled(true);
            ui->joinButton->setEnabled(true);
            ui->connectAnswer->setEnabled(false);
            ui->quiz->append("Niepoprawny kod dostępu\n");
            ui->quiz->setAlignment(Qt::AlignLeft);
        }

     }
   else
    {
        ui->quiz->append(QString::fromUtf8(ba).trimmed());
        ui->quiz->setAlignment(Qt::AlignLeft);
    }

}
/*
 * function sendBtnHit()
 *
 * is responsible for sending to the server the user's answer for question
*/
void CLient::sendBtnHit(){

      QString txt;

     if(ui->a->isChecked() ){
       ui->quiz->append("Zaznaczyłeś odpowiedź:<b> a</b>");
       ui->a->setChecked(false);
       txt = "a";
     }
     if(ui->b->isChecked() ){
       ui->quiz->append("Zaznaczyłeś odpowiedź:<b> b</b>");
       ui->b->setChecked(false);
       txt = "b";
     }
     if(ui->c->isChecked() ){
       ui->quiz->append("Zaznaczyłeś odpowiedź:<b> c</b>");
       ui->c->setChecked(false);
       txt = "c";
     }
     if(ui->d->isChecked() ){
         ui->quiz->append("Zaznaczyłeś odpowiedź:<b> d</b>");
         ui->d->setChecked(false);
         txt = "d";
     }

     sock->write(("?"+txt+'*').toUtf8());
     ui->quiz->setFocus();
}

/*
 *function createQuiz()
 * creats the quiz
 * sends the acces code to the server
 *
 * capacity stores the number of question in this quiz
 * correct_answer[] stores the correct answer for each question
*/
void CLient::createQuiz(){
    //disable button to create a new quiz
    ui->code->setEnabled(false);
    ui->numOfQuest->setEnabled(false);
    ui->create->setEnabled(false);

    //enable the add_question button and section with question
    ui->add->setEnabled(true);
    ui->connectQuestion->setEnabled(true);

    //data needed to create quiz
    quiz1.AccessCode = ui->code->value();
    quiz1.capacity = ui->numOfQuest->value();

    sock->write("#");
    sock->write(QString::number(quiz1.AccessCode).toUtf8());
    sock->write("*");
    sock->write(QString::number(quiz1.capacity).toUtf8());

    ui->join_game->setEnabled(false);
    ui->connectAnswer->setEnabled(false);
    ui->exitButton->setEnabled(true);
}

/*
 *function addQuestion()
 * adds the question to the quiz structure
 * and sends it to the server
 *
 * capacity stores the number of question in this quiz
 * questionTxt[] stores the text of each question
*/
void CLient::addQuestion(){

    for(int i=0; i<quiz1.capacity; i++){

        quiz1.questionTxt[i] = ui->question->displayText().trimmed();
        quiz1.answer[i] = ui->correct->displayText().trimmed();
        ui->quiz->append(quiz1.questionTxt[0]);

        ui->quiz->clear();
        sock->write((ui->question->text().trimmed()+'\n').toUtf8());
        sock->write("*");
        sock->write((ui->correct->text().trimmed()).toUtf8());
        ui->question->clear();
        ui->correct->clear();
    }

    startUp();
}
/*
 * Funkcja join_game dołączająca klienta do gry
 * wysyła żądanie o pytania z quizu z kodem dostępu join_code
 */
void CLient::joinToTheGame(){

    ui->quiz->clear();
    int code = ui->join_code->value();

    ui->join_game->setEnabled(false);
    ui->connectCreate->setEnabled(false);
    ui->connectAnswer->setEnabled(true);

    //ui->quiz->append("Dołączyłeś do gry "+QString::number(code));

    sock->write("&");
    sock->write(QString::number(code).toUtf8());
    sock->write("*");
    dataReceived();
}
