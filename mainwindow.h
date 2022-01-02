#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtNetwork>
#include <QDomDocument>
#include <QtCore>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Valute {
    quint32 numCode;
    QString charCode;
    qint32 nominal;
    QString name;
    double value;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QNetworkAccessManager* manager;
    QMap<QString, Valute> valutes;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void send(QDate);
    void read(QNetworkReply*);
    void change(QString);
    void valChange(QString);
    void dateChanged(QDate);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
