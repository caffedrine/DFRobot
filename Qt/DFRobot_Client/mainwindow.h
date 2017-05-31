#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtQuick/QtQuick>

#include "tcpsocket.h"

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

    void on_sendHellobutton_clicked();
    void on_connectButton_clicked();

    void updateServer();

    void on_sendHelloButton_clicked();
	void on_speedSlider_valueChanged(int value);
	void on_directionSlider_valueChanged(int value);

	void updateSpeedGauge(int val);

private:
    Ui::MainWindow *ui;
    TcpSocket hSocket;

	//Gauge speedometter
	QQuickView *viewGauge;
	QWidget *containerGauge;
};

#endif // MAINWINDOW_H
