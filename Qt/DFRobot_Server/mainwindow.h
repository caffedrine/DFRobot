#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tcpserver.h"
#include "serialconn.h"
#include "../libs/datastructure.h"

#include <QMainWindow>
#include <QtNetwork>
#include <QDateTime>
#include <QtQuick/QtQuick>
#include <QSerialPort>

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

private slots:
	//TCP Socket
	//void setRCstatus(bool connected);
	void on_startServerButton_clicked();
	void on_stopServerButton_clicked();
	void tcpProcessRecvData(QString data);
	void tcpClientConnectionChanged(bool);

	//Serial
	void on_pushButton_connectArduino_clicked();
	void on_pushButton_disconnectArduino_clicked();
	void on_pushButton_updateList_clicked();

	void serialDataReceivingSlot();
	qint64 serialWrite(QString str);	//better for debugging
    void serialSendDataToCar();
	void setSerialPortStatus(bool connected);
	bool serialAttemptReconnect();

	//Motors sliders
	void on_motor1_slider_valueChanged(int value);
	void on_motor2_slider_valueChanged(int value);
	void on_motor3_slider_valueChanged(int value);
	void on_motor4_slider_valueChanged(int value);

	//Buttons and stuff
	void on_pushButton_reverse1_clicked();
    void on_pushButton_reverse2_clicked();
    void on_pushButton_reverse3_clicked();
    void on_pushButton_reverse4_clicked();
    void on_pushButton_minMotors_clicked();
    void on_pushButton_maxMotors_clicked();
    void on_speedSlider_valueChanged(int value);
    void on_leftRightSlider_valueChanged(int value);
    void on_forwardModeCheckBox_toggled(bool checked);
    void on_emergencyPushButton_clicked();

    //Some custom function
    void updateSpeedGauge(int val);

private:
    Ui::MainWindow *ui;

	//Serial port handler
	class SerialConn *serialPort = Q_NULLPTR;
	SerialConn::BaudRate baud = QSerialPort::Baud115200;

	//TCP Server Handler
	class TcpServer *server = Q_NULLPTR;

    //Data structure
    DataStructure *dataStructure = Q_NULLPTR;

    //Need a global variable to store time lapsed between serial write sessions
    qint64 prevMillis = 0;

    //Gauge speedometter
    QQuickView *viewGauge;
    QWidget *containerGauge;

    //Store command to be forwarded
    QString forwardStr = "";
};

#endif // MAINWINDOW_H
