#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//Load speedmetter stuff
	viewGauge = new QQuickView();
	containerGauge = QWidget::createWindowContainer(viewGauge, this);
	containerGauge->setMinimumSize(160, 160);
	containerGauge->setMaximumSize(160, 160);
	containerGauge->setFocusPolicy(Qt::TabFocus);
	viewGauge->setSource(QUrl("../CircularGauge.qml"));
	ui->verticalLayout->addWidget(containerGauge);

	//lOAD JOYSTICK
	QQuickView *view = new QQuickView();
	view->setSource(QUrl("../virtual_joystick.qml"));
	view->setClearBeforeRendering(true);
	view->setColor(QColor(Qt::transparent));
	QQuickItem *root = view->rootObject();
	connect(root, SIGNAL(joystick_moved(double, double)), this, SLOT(joystick_moved(double, double)) );

	// Create a container widget for the QQuickView
	QWidget *container = QWidget::createWindowContainer(view, this);
	container->setMinimumSize(160, 160);
	container->setMaximumSize(160, 160);
	container->setFocusPolicy(Qt::TabFocus);
	ui->joystickLayout->addWidget(container);

    //Init datastructure
    if(dataStructure != Q_NULLPTR)
    {
        delete dataStructure;
        dataStructure = Q_NULLPTR;
    }
    dataStructure = new DataStructure(4);

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
    //*/
}

MainWindow::~MainWindow()
{
    delete ui;

    if(hSocket != Q_NULLPTR)
        delete hSocket;

    if(viewGauge != Q_NULLPTR)
        delete viewGauge;

    if(containerGauge != Q_NULLPTR)
        delete containerGauge;

    if(dataStructure != Q_NULLPTR)
        delete dataStructure;

}

void MainWindow::updateSpeedGauge(int val)
{
	if(val < 0)
		val *=-1;

	if(val > 255)
		val = 255;

	//maping val to interval 0 - 100
	val = (val * 100)/255;

	//Update value
    QObject *rectangle      = viewGauge->rootObject(); //warning: only works when #include <QtQuick/QtQuick>
    QObject *circularGauge  = rectangle->findChild<QObject *>("circular_gauge");

	if(circularGauge)
	{
		circularGauge->setProperty("value", val);
	}
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

void MainWindow::on_connectButton_clicked()
{
    if(hSocket != Q_NULLPTR)
    {
		delete hSocket;
        hSocket = Q_NULLPTR;
    }

	hSocket = new TcpSocket(this);

	hSocket->setHostname(ui->ipTextBox->text());
	hSocket->setPort(ui->portTextBox->text().toInt());

	hSocket->doConnect();
}

void MainWindow::updateServer()
{
    /*
    //Make sure we don't send to much data
	qint64 currMillis = QDateTime::currentMSecsSinceEpoch();
    if(currMillis - prevMillis < 10)
	{
		return;
	}
	prevMillis = currMillis;
    //*/

    if(!hSocket)
    {
        qDebug() << "TCP SEND: FAILED -> Connect to host first...";
        return;
    }

    //For now, we only send motors parameters
    DataStructure::Motor motors[5]; //Motor IDs should start from 1. This is why we have 5 elements;
    int carSteering, carSpeed;

    //Setting up motors IDs
    for(int i=1; i<=4; i++)
        motors[i].id = i;

    //Get values from GUI
    motors[1].speed = ui->m1speedLabel->text().toInt();
    motors[2].speed = ui->m2speedLabel->text().toInt();
    motors[3].speed = ui->m3speedLabel->text().toInt();
    motors[4].speed = ui->m4speedLabel->text().toInt();

    motors[1].direction = ui->m1DirButton->isChecked() ? DataStructure::BACKWARD : DataStructure::FORWARD;
    motors[2].direction = ui->m2DirButton->isChecked() ? DataStructure::BACKWARD : DataStructure::FORWARD;
    motors[3].direction = ui->m3DirButton->isChecked() ? DataStructure::BACKWARD : DataStructure::FORWARD;
    motors[4].direction = ui->m4DirButton->isChecked() ? DataStructure::BACKWARD : DataStructure::FORWARD;

    carSteering = ui->directionLabel->text().toInt();
    carSpeed    = ui->speedLabel->text().toInt();

    //Process data here in order to get clean movement of car
    /////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////
    //Fill values into dataStructure
    for(int i=1; i<=4; i++)
        dataStructure->setMotorInfo(i, motors[i].speed, motors[i].direction);

    //Setting speed and steering
    dataStructure->setSpeed(carSpeed);
    dataStructure->setSteering(carSteering);

//    for(int i=1; i <= 4; i++ )
//        qDebug() << "Motor" << i << ": "
//                 << dataStructure->getMotorInfo(i).id << " - "
//                 << (int)dataStructure->getMotorInfo(i).direction << " - "
//                 <<  dataStructure->getMotorInfo(i).speed;

//    qDebug() << "Speed: " << dataStructure->getSpeed().currentVal << "\tLast speed: " << dataStructure->getSpeed().lastVal;
//    qDebug() << "Steer: " << dataStructure->getSteering().currentVal << "\tLast steer: " << dataStructure->getSteering().lastVal;

    const std::string str = dataStructure->buildDataString(true);
    QString buildedDataString = QString::fromStdString( str );

    if(this->hSocket->isAvailable())
    {
        this->hSocket->write(buildedDataString);

        QString dbgStr = "TCP SEND: (" + QString::number(buildedDataString.size()) + "bytes): ";
        dbgStr += buildedDataString;
        qDebug() << dbgStr;
    }
    else
    {
        qDebug() << "FAILED: Cant write to socket. Is it opened and readable?";
    }
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
	this->ui->speedLabel->setText(QString::number(value));
	updateSpeedGauge(value);

	updateServer();
}

void MainWindow::on_directionSlider_valueChanged(int value)
{
	this->ui->directionLabel->setText(QString::number(value));
	updateSpeedGauge(value);

	updateServer();
}

void MainWindow::on_emergencyButton_clicked()
{
	this->ui->speedSlider->setValue(0);
	this->ui->directionSlider->setValue(0);
	delay(200);
	updateServer();
}

void MainWindow::delay(long milis)
{
	QTime dieTime= QTime::currentTime().addMSecs(milis);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

int map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void MainWindow::joystick_moved(double x, double y)
{
	//qDebug() << "Speed: " << map(x, -1.00000, +1.00000, -255, +255 ) << " " << "Direction: " << map(y, -1.00000, +1.00000, -255, +255 ) << "\n";
	this->ui->speedSlider->setValue( map(y, -1.00000, 1.00000, -255, +255) );
	this->ui->directionSlider->setValue( map(x, -1.00000, 1.00000, -255, +255) );
}

void MainWindow::serverConnectionChanged(bool status)
{
	this->ui->connStatusLabel->setText( status ? "CONNECTED" : "NOT CONNECTED" );
}

void MainWindow::on_disconnectButton_clicked()
{
	if(!hSocket)
			return;

    this->hSocket->doDisconnect();
}

void MainWindow::on_testButton_clicked()
{


    DataStructure data;
    //DataStructure *data = new DataStructure(4); //It may be problematic on Arduino

    data.setMotorInfo(1, 100, DataStructure::FORWARD);
    data.setMotorInfo(2, 200, DataStructure::FORWARD);
    data.setMotorInfo(3, 300, DataStructure::BACKWARD);
    data.setMotorInfo(4, 400, DataStructure::BACKWARD);

    qDebug() << "Motors INFO:";
    for(int i=1; i <= 4; i++)
    {
        qDebug() << "Motor ID: " << data.getMotorInfo(i).id << " Speed: " << data.getMotorInfo(i).speed << " Direction: " << (data.getMotorInfo(i).direction);// == DataStructure::DIRECTION::FORWARD)?"FORWARD":"BACKWARD";
    }

    data.setSpeed(255);
    qDebug() << "Car speed: " << data.getSpeed().currentVal;

    data.setSteering(123);
    qDebug() << "Steering: " << data.getSteering().currentVal;

    const std::string str = data.buildDataString();
    qDebug() << "BUILD: " << QString::fromStdString( str );


    //Proceed with parsing a recv string
    qDebug() << "PARSING: " << ((data.parseDataString( str ))?"SUCCESS":"FAILED");

    qDebug() << "NEW VALUES:";

    qDebug() << "Motors INFO:";
    for(int i=1; i <= 4; i++)
    {
        qDebug() << "Motor ID: " << data.getMotorInfo(i).id << " Speed: " << data.getMotorInfo(i).speed << " Direction: " << (data.getMotorInfo(i).direction);// == DataStructure::DIRECTION::FORWARD)?"FORWARD":"BACKWARD";
    }

    data.setSpeed(255);
    qDebug() << "Car speed: " << data.getSpeed().currentVal;

    data.setSteering(123);
    qDebug() << "Steering: " << data.getSteering().currentVal;

    /* //*/


//    DataStructure *data = new DataStructure(4); //It may be problematic on Arduino

//    data->setMotorInfo(1, 100, DataStructure::FORWARD);
//    data->setMotorInfo(2, 200, DataStructure::FORWARD);
//    data->setMotorInfo(3, 300, DataStructure::BACKWARD);
//    data->setMotorInfo(4, 400, DataStructure::BACKWARD);

//    data->setSpeed(123);
//    data->setSteering(456);

//    const std::string str = data->buildDataString();
//    qDebug() << "BUILD: " << QString::fromStdString( str );
    //*/
}
