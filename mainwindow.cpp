#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    manager = new QNetworkAccessManager(this);
    ui->setupUi(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(read(QNetworkReply*)));
    connect(ui->cbox1, &QComboBox::currentTextChanged,
            this, &MainWindow::change);
    connect(ui->cbox2, &QComboBox::currentTextChanged,
            this, &MainWindow::change);
    ui->lineEdit->setValidator(new QDoubleValidator());
    ui->lineEdit_2->setValidator(new QDoubleValidator());

    ui->dateEdit->setDate(QDate::currentDate());

    connect(ui->dateEdit, &QDateEdit::dateChanged,
            this, &MainWindow::dateChanged);

    connect(ui->lineEdit, &QLineEdit::textChanged,
            this, &MainWindow::valChange);
    connect(ui->lineEdit_2, &QLineEdit::textChanged,
            this, &MainWindow::valChange);
    send(QDate::currentDate());
}

void MainWindow::send(QDate date)
{
    QString day = QString::number(date.day());
    if (day.size() == 1) day.insert(0, '0');
    QString mon = QString::number(date.month());
    if (mon.size() == 1) mon.insert(0, '0');
    QNetworkRequest request;
    QString str = QString("https://www.cbr.ru/scripts/XML_daily.asp?date_req=%1/%2/%3").arg(
                day).arg(mon).arg(date.year());
    qDebug() << str;
    request.setUrl(QUrl(str));
    manager->get(request);
}

void MainWindow::read(QNetworkReply* reply)
{
    valutes.clear();
    QByteArray arr = reply->readAll();
    QString str(arr);
    QXmlStreamReader xmlDoc(QString::fromLatin1(arr));
    while (!xmlDoc.atEnd() && !xmlDoc.hasError()) {
        Valute valute;
        QXmlStreamReader::TokenType token = xmlDoc.readNext();
//        qDebug() << token;
        while (token == QXmlStreamReader::StartElement) {
            if (xmlDoc.name().toString() == "CharCode")
            {
                auto name = xmlDoc.readElementText();
//                qDebug() << name;
                ui->cbox1->addItem(name);
                ui->cbox2->addItem(name);
                valute.charCode = name;
            }
            else if (xmlDoc.name().toString() == "NumCode")
            {
                auto num = xmlDoc.readElementText().toInt();
                valute.numCode = num;
            }
            else if (xmlDoc.name().toString() == "Nominal")
            {
                auto name = xmlDoc.readElementText().toInt();
                valute.nominal = name;
            }
            else if (xmlDoc.name().toString() == "Name")
            {
                auto name = xmlDoc.readElementText();
                valute.name = name;
            }
            else if (xmlDoc.name().toString() == "Value")
            {
                auto name = xmlDoc.readElementText();
                name.replace(",", ".");
//                qDebug() << name;
                valute.value = name.toDouble();
            }
            token = xmlDoc.readNext();
        }
        valutes.insert(valute.charCode, valute);
    }
     auto valute = *valutes.find(ui->cbox1->itemText(0));
     ui->lineRes->setText(QString::number(valute.value));
     ui->lineRes_2->setText(QString::number(valute.value));
     ui->lineEdit->setText("1");
}

void MainWindow::change(QString charCode)
{
    if (sender()->objectName() == "cbox1")
    {
        if (valutes.find(charCode) != valutes.end())
        {
            auto valute = *valutes.find(charCode);
            ui->lineRes->setText(QString::number(valute.value));
            emit ui->lineEdit->textChanged(ui->lineEdit->text());
        }
    }
    else
    {
        if (valutes.find(charCode) != valutes.end())
        {
            auto valute = *valutes.find(charCode);
            ui->lineRes_2->setText(QString::number(valute.value));
            emit ui->lineEdit_2->textChanged(ui->lineEdit_2->text());
        }
    }
}

void MainWindow::valChange(QString val)
{
    static bool change = false;
    auto value = val.toInt();
    auto cb1 = *valutes.find(ui->cbox1->currentText());
    auto cb2 = *valutes.find(ui->cbox2->currentText());

    if (sender()->objectName() == "lineEdit" && !change)
    {
        change = true;
        ui->lineEdit_2->setText(QString::number(value * cb1.value / (cb2.value)));
    }
    if (sender()->objectName() == "lineEdit_2" && !change)
    {
        change = true;
        ui->lineEdit->setText(QString::number(value * cb2.value / (cb1.value)));
    }
    change = false;
}

void MainWindow::dateChanged(QDate date)
{
    send(date);
}

MainWindow::~MainWindow()
{
    delete ui;
}



