#include "client.h"
#include "ui_client.h"

#include <QMessageBox>
#include <QTcpServer>
#include <qtextstream.h>


CLient::CLient(QWidget *parent): QWidget(parent), ui(new Ui::CLient){
    ui->setupUi(this);

    connect(ui->accept, &QPushButton::clicked, this, &CLient::connectBtnHit);
    connect(ui->server, &QLineEdit::returnPressed, ui->accept, &QPushButton::click);
    connect(ui->send, &QPushButton::clicked, this, &CLient::sendBtnHit);
    connect(ui->create, &QPushButton::clicked, this, &CLient::createQuiz);
    connect(ui->add, &QPushButton::clicked, this, &CLient::addQuestion);
    connect(ui->startButton, &QPushButton::clicked, this, &CLient::startUp);
}

CLient::~CLient(){
    sock -> close();
    delete ui;
}
void CLient::startUp(){
 ui->connectMenu->setEnabled(true);
// ui->connect->setEnabled(true);
}

void CLient::connectBtnHit(){
    ui->connectJoin->setEnabled(false);
    ui->add->setEnabled(false);
    ui->quiz->append("<b>Connecting to " + ui->server->text() + ": 2020</b>");

    /* TODO:
     *  - stworzyć gniazdo +
     *  - połączyć zdarzenia z funkcjami je obsługującymi:
     *     • zdarzenie połączenia     (do funkcji socketConnected)
     *     • zdarzenie odbioru danych (stworzyć własną funkcję)
     *     • zdarzenie rozłączenia    (stworzyć własną funkcję)
     *     • zdarzenie błędu          (stworzyć własną funkcję)
     *  - zażądać (asynchronicznego) nawiązania połączenia
     */
    auto * sock = new QTcpSocket;
   /* connect(sock,
            (void (QTcpSocket::*) (QAbstractSocket::SocketError)) &QTcpSocket::error,
            [&]{QMessageBox::critical(this, "Błąd", sock ->errorString());}
            );*/
    connect(sock, &QTcpSocket::connected, this, &CLient::socketConnected);
    connect(sock,
            (void (QTcpSocket::*) (QAbstractSocket::SocketError)) &QTcpSocket::error,
            this,
            &CLient::socketConnected);

    sock -> connectToHost(ui->server->text(), 2020 );

    dataReceived();
    sendBtnHit();

    //rzutowanie na właściwy typ (starego) sygnału error
    //(void (QTcpSocket::*)(QAbstractSocket::SocketError)) &QTcpSocket::error

    connTimeoutTimer = new QTimer(this);
    connTimeoutTimer->setSingleShot(true);
    connect(connTimeoutTimer, &QTimer::timeout, [&]{
        /* TODO: przerwać próbę łączenia się do gniazda */
        connTimeoutTimer->deleteLater();
        ui->connectJoin->setEnabled(true);
        ui->quiz->append("<b>Connect timed out</b>");
        QMessageBox::critical(this, "Error", "Connect timed out");
    });
    connTimeoutTimer->start(3000);
}
void CLient::dataReceived(){
    /*
     * TODO: odczytane z sieci dane można wstawić na pole tekstowe np. za pomocą:
     *
     * QByteArray ba = …
     * ui->msgsTextEdit->append(QString::fromUtf8(ba).trimmed());
     * ui->msgsTextEdit->setAlignment(Qt::AlignLeft);
     */

   /* QByteArray ba = sock->readAll();
    ui->quiz->append(QString::fromUtf8(ba).trimmed());
    ui->quiz->setAlignment(Qt::AlignLeft);
    */
}
void CLient::socketConnected(){

    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    ui->quiz->append("<b>Connected</b>");

}
//popraw
void CLient::sendBtnHit(){
      QString txt;

  /*  Wyślij wtedy gdy masz zaznaczoną odpowiedź
   * if(ui->checkBoxes->setEnabled()) ui->send->setEnabled(true);
       else ui->send->setEnabled(false);  */
     if(ui->a->isChecked() ){
       ui->quiz->append("Zaznaczyłeś odpowiedź:<b> a</b>");
       txt = "a";
       ui->a->setChecked(false);
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

     QByteArray txtAsUtf8 = (txt+'\n').toUtf8();
     //sock->write(txtAsUtf8);
     ui->quiz->clear();
}
/*uzupełnij
    Struktura nie działa
???????????????????????????????
W Utworz quiz trzeba podac ilosc pytan, kolejno kazde pytanie z
odpowiedziami oraz utworzyc kod dostepu. Osoba tworzaca quiz nie może wziac
w nim udzialu*/
void CLient::createQuiz(){

    //connect with server
    sock -> connectToHost(ui->server->text(), 2020 );

    //disable button to create a new quiz
    ui->connectCeate->setEnabled(false);

    //data needed to create quiz
    quiz1.AccessCode = ui->code->value();
    int maxi = ui->numOfQuest->value();
    QString question[maxi];
    QString correct_answers[maxi];

    ui->connectJoin->setEnabled(false);
    ui->connectAnswer->setEnabled(false);

    for (int i=0; i<maxi; i++) {
        quiz1.questionTxt[i] = ui->question->displayText();
        quiz1.answer[i] = ui->correct->displayText();
        ui->quiz->append(quiz1.questionTxt[0]);
    }
    startUp();
}
//uzupełnij
void CLient::addQuestion(){
    ui->quiz->append("pol");
    ui->quiz->append(quiz1.questionTxt[0]);
    QByteArray txtAsUtf8 = (quiz1.questionTxt+'\n')->toUtf8();
    QByteArray answerAsUtf8 = (quiz1.answer+'\n')->toUtf8();
    sock->write(txtAsUtf8);
    sock->write(answerAsUtf8);
}
