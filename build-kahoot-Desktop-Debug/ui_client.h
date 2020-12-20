/********************************************************************************
** Form generated from reading UI file 'client.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CLient
{
public:
    QTextEdit *quiz;
    QGroupBox *connectAnswer;
    QPushButton *send;
    QWidget *widget;
    QHBoxLayout *checkBoxes;
    QRadioButton *a;
    QRadioButton *b;
    QRadioButton *c;
    QRadioButton *d;
    QGroupBox *connectMenu;
    QGroupBox *connectCeate;
    QPushButton *create;
    QSpinBox *code;
    QLabel *label_5;
    QSpinBox *numOfQuest;
    QLabel *label_6;
    QGroupBox *connectJoin;
    QPushButton *accept;
    QLineEdit *server;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *code_join;
    QGroupBox *connectQuestion;
    QPushButton *add;
    QLineEdit *question;
    QLineEdit *correct;
    QLabel *label_7;
    QLabel *label_8;
    QPushButton *startButton;
    QPushButton *exitButton;

    void setupUi(QWidget *CLient)
    {
        if (CLient->objectName().isEmpty())
            CLient->setObjectName(QString::fromUtf8("CLient"));
        CLient->resize(800, 689);
        quiz = new QTextEdit(CLient);
        quiz->setObjectName(QString::fromUtf8("quiz"));
        quiz->setGeometry(QRect(20, 380, 681, 121));
        connectAnswer = new QGroupBox(CLient);
        connectAnswer->setObjectName(QString::fromUtf8("connectAnswer"));
        connectAnswer->setGeometry(QRect(30, 510, 311, 71));
        send = new QPushButton(connectAnswer);
        send->setObjectName(QString::fromUtf8("send"));
        send->setGeometry(QRect(200, 30, 89, 25));
        widget = new QWidget(connectAnswer);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(20, 30, 161, 25));
        checkBoxes = new QHBoxLayout(widget);
        checkBoxes->setObjectName(QString::fromUtf8("checkBoxes"));
        checkBoxes->setContentsMargins(0, 0, 0, 0);
        a = new QRadioButton(widget);
        a->setObjectName(QString::fromUtf8("a"));

        checkBoxes->addWidget(a);

        b = new QRadioButton(widget);
        b->setObjectName(QString::fromUtf8("b"));

        checkBoxes->addWidget(b);

        c = new QRadioButton(widget);
        c->setObjectName(QString::fromUtf8("c"));

        checkBoxes->addWidget(c);

        d = new QRadioButton(widget);
        d->setObjectName(QString::fromUtf8("d"));

        checkBoxes->addWidget(d);

        connectMenu = new QGroupBox(CLient);
        connectMenu->setObjectName(QString::fromUtf8("connectMenu"));
        connectMenu->setGeometry(QRect(20, 0, 681, 371));
        connectCeate = new QGroupBox(connectMenu);
        connectCeate->setObjectName(QString::fromUtf8("connectCeate"));
        connectCeate->setGeometry(QRect(10, 120, 671, 101));
        create = new QPushButton(connectCeate);
        create->setObjectName(QString::fromUtf8("create"));
        create->setGeometry(QRect(560, 60, 91, 31));
        code = new QSpinBox(connectCeate);
        code->setObjectName(QString::fromUtf8("code"));
        code->setGeometry(QRect(250, 60, 111, 31));
        code->setMinimumSize(QSize(111, 31));
        code->setMinimum(0);
        code->setMaximum(9999);
        label_5 = new QLabel(connectCeate);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(250, 30, 67, 17));
        numOfQuest = new QSpinBox(connectCeate);
        numOfQuest->setObjectName(QString::fromUtf8("numOfQuest"));
        numOfQuest->setGeometry(QRect(10, 60, 111, 31));
        numOfQuest->setMinimumSize(QSize(111, 31));
        numOfQuest->setMinimum(1);
        numOfQuest->setMaximum(10);
        numOfQuest->setValue(1);
        label_6 = new QLabel(connectCeate);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 30, 161, 17));
        connectJoin = new QGroupBox(connectMenu);
        connectJoin->setObjectName(QString::fromUtf8("connectJoin"));
        connectJoin->setGeometry(QRect(10, 30, 671, 91));
        accept = new QPushButton(connectJoin);
        accept->setObjectName(QString::fromUtf8("accept"));
        accept->setGeometry(QRect(560, 50, 91, 31));
        server = new QLineEdit(connectJoin);
        server->setObjectName(QString::fromUtf8("server"));
        server->setGeometry(QRect(10, 50, 271, 31));
        label = new QLabel(connectJoin);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 30, 67, 17));
        label_2 = new QLabel(connectJoin);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(310, 30, 67, 17));
        code_join = new QSpinBox(connectJoin);
        code_join->setObjectName(QString::fromUtf8("code_join"));
        code_join->setGeometry(QRect(310, 50, 111, 31));
        code_join->setMinimumSize(QSize(111, 31));
        code_join->setMinimum(0);
        code_join->setMaximum(9999);
        connectQuestion = new QGroupBox(connectMenu);
        connectQuestion->setObjectName(QString::fromUtf8("connectQuestion"));
        connectQuestion->setGeometry(QRect(20, 230, 651, 141));
        add = new QPushButton(connectQuestion);
        add->setObjectName(QString::fromUtf8("add"));
        add->setGeometry(QRect(550, 100, 91, 31));
        question = new QLineEdit(connectQuestion);
        question->setObjectName(QString::fromUtf8("question"));
        question->setGeometry(QRect(10, 50, 631, 41));
        correct = new QLineEdit(connectQuestion);
        correct->setObjectName(QString::fromUtf8("correct"));
        correct->setGeometry(QRect(140, 100, 131, 31));
        label_7 = new QLabel(connectQuestion);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 106, 121, 21));
        label_8 = new QLabel(connectQuestion);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 30, 71, 17));
        startButton = new QPushButton(CLient);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setGeometry(QRect(530, 570, 89, 25));
        exitButton = new QPushButton(CLient);
        exitButton->setObjectName(QString::fromUtf8("exitButton"));
        exitButton->setGeometry(QRect(620, 570, 89, 25));

        retranslateUi(CLient);

        QMetaObject::connectSlotsByName(CLient);
    } // setupUi

    void retranslateUi(QWidget *CLient)
    {
        CLient->setWindowTitle(QApplication::translate("CLient", "CLient", nullptr));
        connectAnswer->setTitle(QApplication::translate("CLient", "Answer", nullptr));
        send->setText(QApplication::translate("CLient", "Send", nullptr));
        a->setText(QApplication::translate("CLient", "A", nullptr));
        b->setText(QApplication::translate("CLient", "B", nullptr));
        c->setText(QApplication::translate("CLient", "C", nullptr));
        d->setText(QApplication::translate("CLient", "D", nullptr));
        connectMenu->setTitle(QApplication::translate("CLient", "Menu", nullptr));
        connectCeate->setTitle(QApplication::translate("CLient", "Create", nullptr));
        create->setText(QApplication::translate("CLient", "Create Quiz", nullptr));
#ifndef QT_NO_TOOLTIP
        code->setToolTip(QApplication::translate("CLient", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label_5->setText(QApplication::translate("CLient", "Code", nullptr));
#ifndef QT_NO_TOOLTIP
        numOfQuest->setToolTip(QApplication::translate("CLient", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("CLient", "Number of questions", nullptr));
        connectJoin->setTitle(QApplication::translate("CLient", "Join", nullptr));
        accept->setText(QApplication::translate("CLient", "Join", nullptr));
        server->setText(QApplication::translate("CLient", "localhost", nullptr));
        label->setText(QApplication::translate("CLient", "Server", nullptr));
        label_2->setText(QApplication::translate("CLient", "Code", nullptr));
#ifndef QT_NO_TOOLTIP
        code_join->setToolTip(QApplication::translate("CLient", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        connectQuestion->setTitle(QApplication::translate("CLient", "Add question", nullptr));
        add->setText(QApplication::translate("CLient", "Add", nullptr));
        question->setText(QString());
        correct->setText(QString());
        label_7->setText(QApplication::translate("CLient", "Correct answer", nullptr));
        label_8->setText(QApplication::translate("CLient", "Question", nullptr));
        startButton->setText(QApplication::translate("CLient", "Start", nullptr));
        exitButton->setText(QApplication::translate("CLient", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CLient: public Ui_CLient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_H
