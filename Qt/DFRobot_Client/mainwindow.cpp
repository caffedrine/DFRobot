#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
    // Autofill ip address field when working on localhost
    QString ipAddress;
    QList<QHostAddress> ips = QNetworkInterface::allAddresses();

    foreach(QHostAddress ip, ips)
    {
        //Get a valid local IP address; localhost is excluded;
        if(ip != QHostAddress::LocalHost && ip.toIPv4Address())
        {
            ipAddress = ip.toString();
        }
    }

    if(!ipAddress.isEmpty())
        ui->ipTextBox->setText(ipAddress);
    else
        ui->ipTextBox->setText("localhost");
    */
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_m1slider_valueChanged(int value)
{
    ui->m1speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_m2slider_valueChanged(int value)
{
    ui->m2speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_m3slider_valueChanged(int value)
{
    ui->m3speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_m4slider_valueChanged(int value)
{
    ui->m4speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_sendHellobutton_clicked()
{

}

void MainWindow::on_connectButton_clicked()
{
    hSocket.setHostname(ui->ipTextBox->text());
    hSocket.setPort(ui->portTextBox->text().toInt());

    hSocket.doConnect();
}

void MainWindow::on_sendHelloButton_clicked()
{
    this->hSocket.write("Hello, once again!");
}

void MainWindow::updateServer()
{
    //Build the string we want to send via network
    //I will use JSON

    //For now, we only send motors parameters
    QString m1s, m1d, m2s, m2d, m3s, m3d, m4s, m4d; //Motors, directions and speeds

    m1s = ui->m1speedLabel->text();
    m2s = ui->m2speedLabel->text();
    m3s = ui->m3speedLabel->text();
    m4s = ui->m4speedLabel->text();

    m1d = ui->m1DirButton->isChecked() ? "1" : "0";
    m2d = ui->m2DirButton->isChecked() ? "1" : "0";
    m3d = ui->m3DirButton->isChecked() ? "1" : "0";
    m4d = ui->m4DirButton->isChecked() ? "1" : "0";

    QString data;
    data += "[" + m1s + "|" + m1d + "],";
    data += "[" + m2s + "|" + m2d + "],";
    data += "[" + m3s + "|" + m3d + "],";
    data += "[" + m4s + "|" + m4d + "]";

    qDebug() << data;

    if(this->hSocket.isAvailable())
        this->hSocket.write(data);
    else
        qDebug() << "FAILED: Cant write to socket. Is it opened and readable?";
}
