#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QDateTime>
#include <QtQuick/QtQuick>
#include <QtMath>

#include "tcpsocket.h"
#include "../libs/datastructure.h"

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
    //Main menu buttons
    void on_actionExit_triggered();

    //Update labels with the value of sliders
    void on_m1slider_valueChanged(int value);
    void on_m2slider_valueChanged(int value);
    void on_m3slider_valueChanged(int value);
    void on_m4slider_valueChanged(int value);

    //Buttons signals
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();

    //Direction-Speed sliders signals
	void on_speedSlider_valueChanged(int value);
	void on_directionSlider_valueChanged(int value);
	void on_leftRightSlider_valueChanged(int value);
	void on_emergencyButton_clicked();

	//Joystick movement slot
	void joystick_moved(double x, double y);

	//Methods
	void updateServer();
	qint64 tcpWrite(QString data);
	void updateSpeedGauge(int val);

    void on_testButton_clicked();

	void on_checkBox_controlIndependentMotors_toggled(bool checked);

public slots:
	void serverConnectionChanged(bool);

private:
    Ui::MainWindow *ui;
	class TcpSocket *hSocket = Q_NULLPTR;

	//Gauge speedometter
	QQuickView *viewGauge;
	QWidget *containerGauge;

    //Data structure handler
    DataStructure *dataStructure = Q_NULLPTR;

	//Need a global variable to store time lapsed between serial write sessions
	qint64 prevMillis = 0;

    //A variable to sync data
    bool canUpdateServer = true;

};

#endif // MAINWINDOW_H
