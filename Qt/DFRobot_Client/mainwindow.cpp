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
	/*
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

//      ___    __  __   _
//     / _ \  |  \/  | | |
//    | | | | | |\/| | | |
//    | |_| | | |  | | | |___
//     \__\_\ |_|  |_| |_____|
//  QML Related methods and stuff

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

void MainWindow::joystick_moved(double x, double y)
{
    //We need a function to mmap some values
    auto map = [](double x, double in_min, double in_max, double out_min, double out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    };

    //We don't want to get maximum speed. It's risky ;)
	int speedMin = -150, speedMax = 150;
	int steerMin = -90, steerMax = 90;

    //qDebug() << "Speed: " << map(x, -1.00000, +1.00000, -255, +255 ) << " " << "Direction: " << map(y, -1.00000, +1.00000, -255, +255 ) << "\n";

    canUpdateServer = false;
    this->ui->speedSlider->setValue( map(y, -1.00000, 1.00000, speedMin, speedMax) );
    this->ui->directionSlider->setValue( map(x, -1.00000, 1.00000, steerMin, steerMax) );
    canUpdateServer = true;

    updateServer();
}
/*
//     _____  ____  ____    __ ___  ____
//    |_   _|/ ___||  _ \  / /|_ _||  _ \
//      | | | |    | |_) |/ /  | | | |_) |
//      | | | |___ |  __// /   | | |  __/
//      |_|  \____||_|  /_/   |___||_|
//  TCP/IP Connection methods and related here
*/
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

void MainWindow::on_disconnectButton_clicked()
{
    if(!hSocket)
            return;

    this->hSocket->doDisconnect();
}

void MainWindow::updateServer()
{
    //Make sure we don't send to much data
//	qint64 currMillis = QDateTime::currentMSecsSinceEpoch();
//    if(currMillis - prevMillis < 10)
//	{
//		return;
//	}
//	prevMillis = currMillis;
//    //*/

//    if(!hSocket)
//    {
//        qDebug() << "TCP SEND: FAILED -> Connect to host first...";
//        return;
//    }

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
	///
	int m1Offset,   m2Offset,   m3Offset,   m4Offset;
	int m1Speed,    m2Speed,    m3Speed,    m4Speed;
	int m1Dir,      m2Dir,      m3Dir,      m4Dir;

	int direction = 0;
	int speed   = 0;

	// Read offsets for motor calibration
	m1Offset = motors[1].speed;
	m2Offset = motors[2].speed;
	m3Offset = motors[3].speed;
	m4Offset = motors[4].speed;

	// Read direction of rotation and intensyty
	direction = carSteering;
	// Read speed and direction
	speed = carSpeed;

	//Processing speed and stuff
	//Setting up direction. Negative value means backward
	if(speed < 0)
	{
		m1Dir = m2Dir = m3Dir = m4Dir = 0;
		speed *= -1;
	}
	else
		m1Dir = m2Dir = m3Dir = m4Dir = 1;

	// Set all speed
	m1Speed = m2Speed = m3Speed = m4Speed = speed;

	if( !direction )
	{
		/*
			If make motor as array:
			int motors[motorsNumber];
			int offsets[motorsNumber];

			for(int i = 0, i < motorsNumber, i++ )
			{
				motors[i] = speed;
				if( speed )
				{
					motors[i] = m1Speed + offsets[0];
					if( motors[i] > 255 )
						motors[i] = 255;
				}
			}

		*/

		if(speed)
		{
			m1Speed = m1Offset + speed;
			if( m1Speed > 255 )
				m1Speed = 255;

			m2Speed = m2Offset + speed;
			if( m2Speed > 255 )
				m2Speed = 255;

			m3Speed = m3Offset + speed;
			if( m3Speed > 255 )
				m3Speed = 255;

			m4Speed = m4Offset + speed;
			if( m4Speed > 255 )
				m4Speed = 255;
		}

//            //We may want to add offsets
//            if(     m1Speed + m1Offset > 255 ||
//                    m2Speed + m2Offset > 255 ||
//                    m3Speed + m3Offset > 255 ||
//                    m4Speed + m4Offset > 255    )
//            {
//                //How much more
//                if( m1Speed + m1Offset > 255    )
//                {
//                    overVal = (m1Speed + m1Offset) - 255;
//                    m1Speed = m1Offset + speed - overVal;
//                    //Or you can simply m1Speed = 255 if yuou don't like math	.
//                }
//                else if(m2Speed + m2Offset > 255)
//                {
//                    overVal = (m2Speed + m2Offset) - 255;
//                    m2Speed = m2Offset + speed - overVal;
//                }
//                else if(m3Speed + m3Offset > 255)
//                {
//                    overVal = (m3Speed + m3Offset) - 255;
//                    m3Speed = m3Offset + speed - overVal;
//                }
//                else if(m4Speed + m4Offset > 255)
//                {
//                    overVal = (m4Speed + m4Offset) - 255;
//                    m4Speed = m4Offset + speed - overVal;
//                }
//            }
//            // ???????
//            else
//            {
//                m1Speed = speed + m1Offset;
//                m2Speed = speed + m2Offset;
//                m3Speed = speed + m3Offset;
//                m4Speed = speed + m4Offset;
//            }

		// ****************** ???????????????
//            if( !(m1Speed + m1Offset > 255) )
//                m1Speed -= overVal;

//            if( !(m2Speed + m2Offset > 255) )
//                m2Speed -= overVal;

//            if( !(m3Speed + m3Offset > 255) )
//                m3Speed -= overVal;

//            if( !(m4Speed + m4Offset > 255) )
//                m4Speed -= overVal;
		// ****************** **************

//            //hOWEVER, IF SPEED = 0, we want breake
//            if(speed == 0)
//                m1Speed = m2Speed = m3Speed = m4Speed = 0;

		// *******************************
	}
	else
	{
	//Process direction
	//Check if direction is changed and change motors values again
//		direction = this->ui->leftRightSlider->value();

//		if(direction != 0)
//		{
		//this mean that we have to change direction of pair motors
		//M1 = M4
		//M2 = M3

		bool minus = false;

		if(direction < 0)
		{
			//We need positive direction as it is easier to work with
			direction *= -1;
			minus = true;
		}

		int hSpeed = speed;
		int lSpeed = ( speed * ( 100 - direction ) ) / 100;

		if( minus )
		{
//				//We need positive direction as it is easier to work with
//				direction *= -1;

			m1Speed = m4Speed = lSpeed;
			m2Speed = m3Speed = hSpeed;

//				m1Speed -= (direction/2);
//				if(m1Speed < 0)
//				{
//					m1Speed *=-1;
//					m1Dir = (m1Dir == 0)?1:0;
//				}

//				m4Speed -= (direction/2);
//				if(m4Speed < 0)
//				{
//					m4Speed *=-1;
//					m4Dir = (m4Dir == 0)?1:0;
//				}

//				m2Speed += (direction/2);
//				if(m2Speed > 255)
//					m2Speed = 255;

//				m3Speed += (direction/2);
//				if(m3Speed > 255)
//					m3Speed = 255;
		}
		else
		{
			m1Speed = m4Speed = hSpeed;
			m2Speed = m3Speed = lSpeed;

//				//we have to move right with k coeficient = direction
//				m1Speed += (direction/2);
//				if(m1Speed > 255)
//					m1Speed = 255;

//				m4Speed += (direction/2);
//				if(m4Speed > 255)
//					m4Speed = 255;

//				m2Speed -= (direction/2);
//				if(m2Speed < 0)
//				{
//					m2Speed *=-1;
//					m2Dir = (m2Dir == 0)?1:0;
//				}

//				m3Speed -= (direction/2);
//				if(m3Speed < 0)
//				{
//					m3Speed *=-1;
//					m3Dir = (m3Dir == 0)?1:0;
//				}
		}
	}

	motors[1].speed = m1Speed;
	motors[1].direction = (DataStructure::DIRECTION)m1Dir;

	motors[2].speed = m2Speed;
	motors[2].direction = (DataStructure::DIRECTION)m2Dir;

	motors[3].speed = m3Speed;
	motors[3].direction = (DataStructure::DIRECTION)m3Dir;

	motors[4].speed = m4Speed;
	motors[4].direction = (DataStructure::DIRECTION)m4Dir;

    /////////////////////////////////////////////////////////
    //Fill values into dataStructure
	for(int i=1; i<=4; i++)
        dataStructure->setMotorInfo(i, motors[i].speed, motors[i].direction);

	//Setting speed and steering
	///We process those data here. So the car will expect only values for motors.
	//dataStructure->setSpeed(carSpeed);
	//dataStructure->setSteering(carSteering);

    //Update speed gauge here so have as accurate info as possible
	updateSpeedGauge(carSpeed);

    const std::string str = dataStructure->buildDataString(true);
    QString buildedDataString = QString::fromStdString( str );
    if(buildedDataString.length() < 6 )
        return;


    if(this->hSocket != Q_NULLPTR && this->hSocket->isAvailable())
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

void MainWindow::serverConnectionChanged(bool status)
{
    this->ui->connStatusLabel->setText( status ? "CONNECTED" : "NOT CONNECTED" );
}

//	  ____  _   _  ___
//	 / ___|| | | ||_ _|
//	| |  _ | | | | | |
//	| |_| || |_| | | |
//	 \____| \___/ |___|
//	GUI Related methods

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_m1slider_valueChanged(int value)
{
    ui->m1speedLabel->setText( QString::number(value) );

    if(canUpdateServer)
        updateServer();
}

void MainWindow::on_m2slider_valueChanged(int value)
{
    ui->m2speedLabel->setText( QString::number(value) );

    if(canUpdateServer)
        updateServer();
}

void MainWindow::on_m3slider_valueChanged(int value)
{
    ui->m3speedLabel->setText( QString::number(value) );

    if(canUpdateServer)
        updateServer();
}

void MainWindow::on_m4slider_valueChanged(int value)
{
    ui->m4speedLabel->setText( QString::number(value) );

    if(canUpdateServer)
        updateServer();
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
	this->ui->speedLabel->setText(QString::number(value));

    if(canUpdateServer)
        updateServer();
}

void MainWindow::on_directionSlider_valueChanged(int value)
{
	this->ui->directionLabel->setText(QString::number(value));

    if(canUpdateServer)
        updateServer();
}

void MainWindow::on_emergencyButton_clicked()
{
    canUpdateServer = false;    //Prevent sending too much data. We want to send booth changes at the same time

    this->ui->speedSlider->setValue(0);
	this->ui->directionSlider->setValue(0);
	this->ui->leftRightSlider->setValue(0);

    canUpdateServer = true;

	updateServer();
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

void MainWindow::on_leftRightSlider_valueChanged(int value)
{
	ui->leftRightLabel->setText( QString::number(value) );

	//This slider makes car to move horizontally.
	DataStructure::Motor motors[5]; //Motor IDs should start from 1. This is why we have 5 elements;
	int speed = value;

	//Setting up motors IDs
	for(int i=1; i<=4; i++)
		motors[i].id = i;

	//Get values from GUI
	motors[1].speed = ui->m1speedLabel->text().toInt();
	motors[2].speed = ui->m2speedLabel->text().toInt();
	motors[3].speed = ui->m3speedLabel->text().toInt();
	motors[4].speed = ui->m4speedLabel->text().toInt();

	if(speed < 0)
	{
		speed *= -1;

		///CAR should be moved on left side
		//setting up motors directions
		motors[1].direction = motors[3].direction = DataStructure::FORWARD;
		motors[2].direction = motors[4].direction = DataStructure::BACKWARD;

		//Setting up motors speed + offset
		for(int i=1; i<= 4; i++)
			motors[i].speed = speed + motors[i].speed;	//because we already have offset stored here
	}
	else if(speed > 0)
	{
		///CAR should be moved on left side
		//setting up motors directions
		motors[1].direction = motors[3].direction = DataStructure::BACKWARD;
		motors[2].direction = motors[4].direction = DataStructure::FORWARD;

		//Setting up motors speed + offset
		for(int i=1; i<= 4; i++)
			motors[i].speed = speed + motors[i].speed;	//because we already have offset stored here
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//Sending motors values to car//Fill values into dataStructure
	for(int i=1; i<=4; i++)
		dataStructure->setMotorInfo(i, motors[i].speed, motors[i].direction);	//Fill values in data structure so we can parse'em

	const std::string str = dataStructure->buildDataString(true);
	QString buildedDataString = QString::fromStdString( str );
	if(buildedDataString.length() < 6 )
		return;

	if(this->hSocket != Q_NULLPTR && this->hSocket->isAvailable())
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
