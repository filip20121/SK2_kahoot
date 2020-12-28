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

}

void CLient::connectBtnHit(){
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

    sock -> connectToHost(ui->server->text(), ui->port->value() );
    connTimeoutTimer->start(3000);
}
void CLient::dataReceived(){
   //  mutex.lock();
    QByteArray ba = sock->readAll();
    ui->quiz->append(QString::fromUtf8(ba).trimmed());
    ui->quiz->setAlignment(Qt::AlignLeft);

    if(QString::fromUtf8(ba).trimmed() == "end"){
       ui->exitButton->setEnabled(true);
    }
     //mutex.unlock();

}
void CLient::socketConnected(){

    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    ui->quiz->append("<b>Connected</b>");

}

void CLient::socketDisconnected(){
    ui->quiz->append("<b>Disconnected</b>");
    ui->connect->setEnabled(true);
}

void CLient::socketError(QTcpSocket::SocketError err){
    if(err == QTcpSocket::RemoteHostClosedError)
        return;
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->quiz->append("<b>Socket error: "+sock->errorString()+"</b>");
    ui->connect->setEnabled(true);
}


//popraw
void CLient::sendBtnHit(){
  //  mutex.lock();
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
     //
     sock->write((txt+'\n').toUtf8());
    // ui->quiz->clear();
     ui->quiz->setFocus();
  //  mutex.unlock();
}
/*uzupełnij

???????????????????????????????
W Utworz quiz trzeba podac kolejno kazde pytanie z
odpowiedziami oraz utworzyc kod dostepu. Osoba tworzaca quiz nie może wziac
w nim udzialu*/
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
    QString question[quiz1.capacity];
    QString correct_answers[quiz1.capacity];

    ui->join_game->setEnabled(false);
    ui->connectAnswer->setEnabled(false);
    ui->exitButton->setEnabled(true);
}
//uzupełnij
void CLient::addQuestion(){

    for(int i=0; i<quiz1.capacity; i++){

        quiz1.questionTxt[i] = ui->question->displayText().trimmed();
        quiz1.answer[i] = ui->correct->displayText().trimmed();
        ui->quiz->append(quiz1.questionTxt[0]);

        ui->quiz->clear();
        //sock->write((quiz1.questionTxt[i]+'\n').toUtf8());
        sock->write((ui->question->text().trimmed()+'\n').toUtf8());
        //sock->write((quiz1.answer[i]+'\n').toUtf8());
        sock->write((ui->correct->text().trimmed()+'\n').toUtf8());
    }
    ui->question->clear();
    ui->correct->clear();
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

    ui->quiz->append("Dołączyłeś do gry "+QString::number(code));

    sock->write(QString::number(code).toUtf8());
    dataReceived();
}
