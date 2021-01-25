#include "client.h"
#include "ui_client.h"

#include <QMessageBox>
#include <QTcpServer>
#include <qtextstream.h>
#include <QString>
#include <QMutex>

QMutex mutex;

Client::Client(QWidget *parent): QWidget(parent), ui(new Ui::Client){
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &Client::connectBtnHit);
    connect(ui->server, &QLineEdit::returnPressed, ui->connectButton, &QPushButton::click);
    connect(ui->send, &QPushButton::clicked, this, &Client::sendBtnHit);
    connect(ui->create, &QPushButton::clicked, this, &Client::createQuiz);
    connect(ui->add, &QPushButton::clicked, this, &Client::addQuestion);
    connect(ui->undoButton, &QPushButton::clicked, this, &Client::undoSetDisable);
    connect(ui->joinButton, &QPushButton::clicked, this, &Client::joinToTheGame);
    connect(ui->startButton, &QPushButton::clicked, this, &Client::startTheGame);
}

Client::~Client(){
    sock -> close();
    delete ui;
}
void Client::undoSetDisable(){

    ui->connectMenu->setEnabled(true);
    ui->join_game->setEnabled(true);
    ui->connectCreate->setEnabled(true);
    ui->create->setEnabled(true);
    ui->numOfQuest->setEnabled(true);
    ui->code->setEnabled(true);
    ui->connectQuestion->setEnabled(true);
    ui->start_game->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->time->setEnabled(true);
    ui->quiz->clear();

}
void Client::onlyConnect(){
    ui->join_game->setEnabled(false);
    ui->connectCreate->setEnabled(false);
    ui->connectQuestion->setEnabled(false);
    ui->start_game->setEnabled(false);
}

void Client::connectBtnHit(){
    ui->quiz->clear();
    ui->connect->setEnabled(false);
    ui->join_game->setEnabled(true);
    ui->connectCreate->setEnabled(true);
    ui->connectQuestion->setEnabled(false);
    ui->start_game->setEnabled(true);

    ui->quiz->append("<b>Connecting to " + ui->server->text() + ":" + ui->port->value() + "</b>");
    sock = new QTcpSocket(this);
        connTimeoutTimer = new QTimer(this);
        connTimeoutTimer->setSingleShot(true);

    connect(connTimeoutTimer, &QTimer::timeout, [&]{
            sock->abort();
            sock->deleteLater();
            connTimeoutTimer->deleteLater();
            onlyConnect();
            ui->quiz->append("<b>Connect timed out</b>");
            QMessageBox::critical(this, "Error", "Connect timed out");
        });

    connect(sock, &QTcpSocket::connected, this, &Client::socketConnected);
    connect(sock, &QTcpSocket::disconnected, this, &Client::socketDisconnected);
    connect(sock, (void(QTcpSocket::*)(QTcpSocket::SocketError)) &QTcpSocket::error, this, &Client::socketError);
    connect(sock, &QTcpSocket::readyRead, this, &Client::dataReceived);

    sock -> connectToHost(ui->server->text(), ui->port->value());
    connTimeoutTimer->start(3000);
}

void Client::socketConnected(){
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    ui->quiz->append("<b>Connected</b>");

}

void Client::socketDisconnected(){
    ui->quiz->append("<b>Disconnected</b>");
    ui->connect->setEnabled(true);
    onlyConnect();
}

void Client::socketError(QTcpSocket::SocketError err){
    if(err == QTcpSocket::RemoteHostClosedError)
        return;
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->quiz->append("<b>Socket error: "+sock->errorString()+"</b>");
    ui->connect->setEnabled(true);
    onlyConnect();
}

/*
 * function dataReceived()
 *
 * is responsible for reading data from the server
 * data is like question with posssible answers,
 *              your score,
*/
void Client::dataReceived(){
    QByteArray ba = sock->readAll();

     if(QString::fromUtf8(ba)[0] == "!"){
        //if quiz ended
        if(QString::fromUtf8(ba)[1] == "#")
        {
            ui->undoButton->setEnabled(true);
            ui->quiz->append("Quiz się zakończył.\n");
            ui->quiz->setAlignment(Qt::AlignLeft);
            ui->startButton->setEnabled(true);

        }
        //if code is incorrect whlie joining
        if(QString::fromUtf8(ba)[1] == "*")
        {
            ui->join_game->setEnabled(true);
            ui->joinButton->setEnabled(true);
            ui->connectAnswer->setEnabled(false);
            ui->quiz->append("Niepoprawny kod dostępu.\n");
            ui->quiz->setAlignment(Qt::AlignLeft);
        }
        //if code is already used while creating
        if(QString::fromUtf8(ba)[1] == "?"){
            ui->quiz->append("Podany kod jest już zajęty. Podaj inny.\n");
            ui->connectCreate->setEnabled(true);
            ui->create->setEnabled(true);
            ui->code->setEnabled(true);
            ui->numOfQuest->setEnabled(true);
            ui->connectQuestion->setEnabled(false);
        }

     }
   else
    {
        ui->quiz->clear();
        ui->quiz->append(QString::fromUtf8(ba).trimmed());
        ui->quiz->setAlignment(Qt::AlignLeft);
        ui->undoButton->setEnabled(true);
    }

}
/*
 * function sendBtnHit()
 *
 * is responsible for sending to the server the user's answer for question
*/
void Client::sendBtnHit(){

      QString txt="";

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

     sock->write(("?"+txt).toUtf8());
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
void Client::createQuiz(){
    //disable button to create a new quiz
    ui->code->setEnabled(false);
    ui->numOfQuest->setEnabled(false);
    ui->time->setEnabled(false);
    ui->create->setEnabled(false);
    ui->start_game->setEnabled(false);

    //enable the add_question button and section with question
    ui->add->setEnabled(true);
    ui->connectQuestion->setEnabled(true);
    numOfQuestions = ui->numOfQuest->value();
    code = ui->code->value();
    time = ui->time->value();

    //data needed to create quiz
    sock->write("#");
    sock->write(QString::number(code).toUtf8());
    sock->write("#");
    sock->write(QString::number(numOfQuestions).toUtf8());
    sock->write("#");
    sock->write(QString::number(time).toUtf8());
    sock->write("#");

    ui->connectAnswer->setEnabled(false);
    ui->undoButton->setEnabled(true);
    ui->start_game->setEnabled(true);
    ui->startButton->setEnabled(true);

    count = 0;


}

/*
 *function addQuestion()
 * adds the question to the quiz structure
 * and sends it to the server
 *
 * capacity stores the number of question in this quiz
 * questionTxt[] stores the text of each question
*/
void Client::addQuestion(){  
        //ui->quiz->clear();
        sendQuestion = ui->question->text();

        sock->write(sendQuestion.toUtf8());
        sock->write("*");
        sock->write((ui->correct->text().trimmed()).toUtf8());
        sock->write("*");

        ui->question->clear();
        ui->correct->clear();

        if(count+1 >= numOfQuestions){
            undoSetDisable();
            count = 0;
        }
        else{
          count += 1;
        }

}
/*
 * Funkcja join_game dołączająca klienta do gry
 * wysyła żądanie o pytania z quizu z kodem dostępu join_code
 */
void Client::joinToTheGame(){

    ui->quiz->clear();
    int code = ui->join_code->value();

    ui->join_game->setEnabled(false);
    ui->connectCreate->setEnabled(false);
    ui->connectAnswer->setEnabled(true);
    ui->start_game->setEnabled(false);

    sock->write("&");
    sock->write(QString::number(code).toUtf8());
    sock->write("&");
    dataReceived();
}

/*
 * Funkcja join_game dołączająca klienta do gry
 * wysyła żądanie o pytania z quizu z kodem dostępu join_code
 */
void Client::startTheGame(){

    ui->join_game->setEnabled(false);
    ui->quiz->clear();
    int code = ui->start_code->value();
    ui->start_game->setEnabled(false);
    ui->connectCreate->setEnabled(false);
    ui->connectAnswer->setEnabled(false);

    sock->write("@");
    sock->write(QString::number(code).toUtf8());
    ui->startButton->setEnabled(false);
    dataReceived();
}
