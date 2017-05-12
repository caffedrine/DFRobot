#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tcpserver.h"

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtNetwork>
#include <QDateTime>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void processRecvData(QString data);
    void clientConnectionChanged(bool);

private slots:
    void on_pushButton_connectArduino_clicked();
    void on_pushButton_disconnectArduino_clicked();
    void on_pushButton_updateList_clicked();

    void serialDataReceivingSlot();
    void serialConnectionChanged( QSerialPort::SerialPortError );
    bool serialWrite(QString str);
    void serialSendDataToCar();

    void on_motor1_slider_valueChanged(int value);
    void on_motor2_slider_valueChanged(int value);
    void on_motor3_slider_valueChanged(int value);
    void on_motor4_slider_valueChanged(int value);

    void setSerialPortStatus(bool connected);
    //void setRCstatus(bool connected);

    void on_startServerButton_clicked();
    void on_stopServerButton_clicked();
    void on_pushButton_reverse1_clicked();
    void on_pushButton_reverse2_clicked();
    void on_pushButton_reverse3_clicked();
    void on_pushButton_reverse4_clicked();
    void on_pushButton_minMotors_clicked();
    void on_pushButton_maxMotors_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *pSerialPort = Q_NULLPTR;
    class TcpServer *server = Q_NULLPTR;

    //Need a global variable to store time lapsed between serial write sessions
    qint64 prevMillis = 0;
};

#endif // MAINWINDOW_H
