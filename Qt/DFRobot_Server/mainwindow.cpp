#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QList>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//Load CircularGauge
    viewGauge = new QQuickView();
    containerGauge = QWidget::createWindowContainer(viewGauge, this);
    containerGauge->setMinimumSize(160, 160);
    containerGauge->setMaximumSize(160, 160);
    containerGauge->setFocusPolicy(Qt::TabFocus);
    viewGauge->setSource(QUrl("../qml/CircularGauge.qml"));
    ui->verticalLayout->addWidget(containerGauge);

	//Startup functions - Auto start TCP/IP server and connect au Arduino on startup
	this->serialAttemptReconnect();
    on_startServerButton_clicked();

    //Init data structure
    dataStructure = new DataStructure(4);
}

MainWindow::~MainWindow()
{
    delete ui;

	if(serialPort != Q_NULLPTR)
		delete serialPort;
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
    QObject *rectangle = viewGauge->rootObject(); //warning: only works when #include <QtQuick/QtQuick>
    QObject *circularGauge = rectangle->findChild<QObject *>("circular_gauge");
    if(circularGauge)
    {
        circularGauge->setProperty("value", val);
    }
}

//   ____               _         _
//	/ ___|   ___  _ __ (_)  __ _ | |
//	\___ \  / _ \| '__|| | / _` || |
//	 ___) ||  __/| |   | || (_| || |
//	|____/  \___||_|   |_| \__,_||_|
//	Methods which interact with serial communication

void MainWindow::on_pushButton_connectArduino_clicked()
{
	//Getting portname and baudrate
	QString portName = ui->comboBox_serialSlots->currentText();

	if(serialPort != Q_NULLPTR)
	{
		if(serialPort->isOpen())
			serialPort->disconnect();

		delete serialPort;
		serialPort = Q_NULLPTR;
	}

	serialPort = new SerialConn();

	//connecting serial signals
	connect(serialPort, SIGNAL(connectionStatusChanged(bool)), this, SLOT(setSerialPortStatus(bool)));
	connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialDataReceivingSlot()));


	if(serialPort->connect(portName, this->baud))
	{
		qDebug() << "SUCCESS: You're now connected to Arduino!";
	}
	else
	{
		qDebug() << "FAILED2CONN: " << serialPort->getLastError();
		if(serialPort != Q_NULLPTR || serialPort)
		{
			delete serialPort;
			serialPort = Q_NULLPTR;
		}
	}

}

void MainWindow::on_pushButton_updateList_clicked()
{
	//Cleaning combolist
	ui->comboBox_serialSlots->clear();

	QList<QString> serialPorts = SerialConn::getSerialPorts();

	//Filling combo list
	foreach (QString serialPort, serialPorts)
	{
		ui->comboBox_serialSlots->addItem(serialPort);
	}

	//Send some debug info
	qDebug() << "SUCCESS: Serial ports list updated!";
}

void MainWindow::on_pushButton_disconnectArduino_clicked()
{
	if(!serialPort)
	{
		qDebug() << "FAILED: You're not connected to Arduino!";
		return;
	}

	if(this->serialPort->disconnect())
	{
		qDebug() << "SUCCESS: DISCONNECTED!";
		this->setSerialPortStatus(0);
	}
	else
	{
		qDebug() << "FAILED: " << serialPort->getLastError();
	}
}

void MainWindow::serialDataReceivingSlot()
{
    // Read data
	QString data = serialPort->readString();

	if(data != "^_^")	//this is just a hack to make sure we get valid data.
	{
		// Print data
		qDebug() << "ARDUINO RECV: " << data;

		//Send it to visual console too
		ui->arduinoConnectionConsole->appendPlainText(data);
	}
}

qint64 MainWindow::serialWrite(QString str)
{
    qint64 currMillis = QDateTime::currentMSecsSinceEpoch();
    if(currMillis - prevMillis < 50)
    {
        return false;
    }
    prevMillis = currMillis;

	if(serialPort == Q_NULLPTR)
	{
		qDebug() << "ARDUINO SEND: Please connect to Arduino board first!";
		return false;
	}

	qint64 bytesWritten = serialPort->write(str);
	if( bytesWritten > 0)
	{
		QString dbgStr = "ARD SUCCESS SEND (" + QString::number(bytesWritten) + "bytes): " + str;
		qDebug() << dbgStr;
	}
	else
	{
		qDebug() << "FAILED: " << serialPort->getLastError();
	}

	return bytesWritten;
}

void MainWindow::setSerialPortStatus(bool connected)
{
	ui->arduinoConnectionStatusLabel->setText( connected ? "CONNECTED" : "NOT CONNECTED" );

	//Check also if server is still connected. In this case all board may have been powered off
	if(server)
	{
		if(!connected)
			server->write("Arduino connection lost :(");
		else
			server->write("Houston, we have connection ^_^");
	}
}

void MainWindow::serialSendDataToCar()
{
	QString msg = ""; // [M1,0,255] - motor, direction, speed

    if(ui->arduinoConnectionStatusLabel->text().contains("NOT"))
        this->serialAttemptReconnect();

	if(ui->offsetCheckBox->isChecked() == false || ui->forwardModeCheckBox->isChecked() == true)
	{
		msg += "[M1,";
		msg += ui->pushButton_reverse1->isChecked() ? "0" : "1";
		msg += ",";
		msg += QString::number(ui->motor1_slider->value());
		msg += "]-";

		msg += "[M2,";
		msg += ui->pushButton_reverse2->isChecked() ? "0" : "1";
		msg += ",";
		msg += QString::number(ui->motor2_slider->value());
		msg += "]-";

		msg += "[M3,";
		msg += ui->pushButton_reverse3->isChecked() ? "0" : "1";
		msg += ",";
		msg += QString::number(ui->motor3_slider->value());
		msg += "]-";

		msg += "[M4,";
		msg += ui->pushButton_reverse4->isChecked() ? "0" : "1";
		msg += ",";
		msg += QString::number(ui->motor4_slider->value());
		msg += "]";
	}
	else
	{
		int m1Offset,   m2Offset,   m3Offset,   m4Offset;
		int m1Speed,    m2Speed,    m3Speed,    m4Speed;
		int m1Dir,      m2Dir,      m3Dir,      m4Dir;

		int direction;
		int speed   = 0;

		// Read offsets for motor calibration
		m1Offset = ui->motor1_speedLabel->text().toInt();
		m2Offset = ui->motor2_speedLabel->text().toInt();
		m3Offset = ui->motor3_speedLabel->text().toInt();
		m4Offset = ui->motor4_speedLabel->text().toInt();

		// Read direction of rotation and intensyty
		direction = ui->directionLabel->text().toInt();
		// Read speed and direction
		speed = ui->speedLabel->text().toInt();

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
		//******************************************

		msg += "[M1,";
		msg += QString::number(m1Dir);
		msg += ",";
		msg += QString::number(m1Speed);
		msg += "]-";

		msg += "[M2,";
		msg += QString::number(m2Dir);
		msg += ",";
		msg += QString::number(m2Speed);
		msg += "]-";

		msg += "[M3,";
		msg += QString::number(m3Dir);
		msg += ",";
		msg += QString::number(m3Speed);
		msg += "]-";

		msg += "[M4,";
		msg += QString::number(m4Dir);
		msg += ",";
		msg += QString::number(m4Speed);
		msg += "]";

	}

	if(serialPort == Q_NULLPTR || !serialPort->isOpen())
	{
		//It will crash anyway if prefix/device is wrong :(
		try
		{
			if(this->serialAttemptReconnect())
				serialWrite(msg);
			else
				qDebug() << "ARD SEND: No Arduino connection!";
		}
		catch(...)
		{
			qDebug() << "ARD SEND: Fatal error on Serial port reconnect (THIS IS BAD)";
		}
	}
	else
	{
		serialWrite(msg);
	}
}

bool MainWindow::serialAttemptReconnect()
{
    if(serialPort != Q_NULLPTR || serialPort)
	{
		delete serialPort;
		serialPort = Q_NULLPTR;
	}

	serialPort = new SerialConn();

	//Update lists in case of change
	this->on_pushButton_updateList_clicked();	//simulate a click on button

	//Select the good index
	QString serialPrefix = "ttyACM";

	//Set list on first serial port with the following prefix
	for(int i=0; i < ui->comboBox_serialSlots->count(); i++)
	{
		if(ui->comboBox_serialSlots->itemText(i).contains(serialPrefix))
			ui->comboBox_serialSlots->setCurrentIndex(i);
	}

	//Call function to reconnect
	QString portName = ui->comboBox_serialSlots->currentText();

	serialPort = new SerialConn();

	//connecting serial signals
	connect(serialPort, SIGNAL(connectionStatusChanged(bool)), this, SLOT(setSerialPortStatus(bool)));
	connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialDataReceivingSlot()));

	if(serialPort->connect(portName, this->baud))
	{
		qDebug() << "SUCCESS: You're now connected to Arduino!";
		return true;
	}
	else
	{
		qDebug() << "FAILED: " << serialPort->getLastError();
		delete serialPort;
		serialPort = Q_NULLPTR;
		return false;
	}
}
/*
//	 _____  ____  ____       __  ___  ____
//	|_   _|/ ___||  _ \     / / |_ _||  _ \
//	  | | | |    | |_) |   / /   | | | |_) |
//	  | | | |___ |  __/   / /    | | |  __/
//	  |_|  \____||_|     /_/    |___||_|
//	Methods which interacts with TCP Sockets
*/
void MainWindow::on_startServerButton_clicked()
{
    //Enable forward mode when listening on network
    ui->forwardModeCheckBox->setChecked(true);

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
        ui->rcConnectionIPAddressTextBox->setText(ipAddress);
    else
        ui->rcConnectionIPAddressTextBox->setText("localhost");

    //Proceed to start the server
    //Note that ip address from text box is just informative. Normally, server will be
    //available from all computer ip addresses, including 127.0.0.1
	if(server != Q_NULLPTR || server)
	{
        delete server;
		server = Q_NULLPTR;
	}

    server = new TcpServer(this);
	connect(server, SIGNAL(tcpProcessRecvData(QString)), this, SLOT(tcpProcessRecvData(QString)) );
	connect(server, SIGNAL(tcpClientConnectionChanged(bool)), this, SLOT(tcpClientConnectionChanged(bool)));

    server->setPort( QString(ui->rcConnectionPortTextBox->text()).toInt() );

    if(server->startServer())
    {
        ui->rcConnectionServerStatus->setText("STARTED");
    }
}

void MainWindow::tcpProcessRecvData(QString data)
{
    data = data.replace("<>", "|");

    ui->rcConnectionConsoleTextBox->appendPlainText(data);
    qDebug() << "TCP RECV: " << data;

    //Append all recv data to our dataStructure
    if(!dataStructure->parseDataString( data.toStdString() ))
    {
		qDebug() << "TCP RECV FAILED: Corrupted packet: " << data;
        return;
    }

    //Update sliders
    ui->speedSlider->setValue( dataStructure->getSpeed().currentVal );
    ui->leftRightSlider->setValue( dataStructure->getSteering().currentVal );

    //Updating motors sliders
    ui->motor1_slider->setValue( dataStructure->getMotorInfo(1).speed );
    ui->motor2_slider->setValue( dataStructure->getMotorInfo(2).speed );
    ui->motor3_slider->setValue( dataStructure->getMotorInfo(3).speed );
    ui->motor4_slider->setValue( dataStructure->getMotorInfo(4).speed );

    //Update buttons
    ui->pushButton_reverse1->setChecked( ((bool)(dataStructure->getMotorInfo(1).direction))?false:true );
    ui->pushButton_reverse2->setChecked( ((bool)(dataStructure->getMotorInfo(2).direction))?false:true );
    ui->pushButton_reverse3->setChecked( ((bool)(dataStructure->getMotorInfo(3).direction))?false:true );
    ui->pushButton_reverse4->setChecked( ((bool)(dataStructure->getMotorInfo(4).direction))?false:true );

    if(ui->forwardModeCheckBox->isChecked())
    {
        //We just forward data to Arduino
        serialSendDataToCar();
    }
}

void MainWindow::on_stopServerButton_clicked()
{
    //Enable forward mode when listening on network
    ui->forwardModeCheckBox->setChecked(false);

    try
    {
        if(this->server != Q_NULLPTR && this->server)
        {
            this->server->stopServer();
            delete server;
            server = Q_NULLPTR;

            ui->rcConnectionServerStatus->setText("STOPPED");
        }
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }
}

void MainWindow::tcpClientConnectionChanged(bool connection)
{
    ui->rcConnectionClientStatus->setText( connection ? "CONNECTED" : "NOT CONNECTED" );
    if(!connection)
    {
        on_emergencyPushButton_clicked();
    }

}

//	  ____  _   _  ___
//	 / ___|| | | ||_ _|
//	| |  _ | | | | | |
//	| |_| || |_| | | |
//	 \____| \___/ |___|
//	GUI Related methods

void MainWindow::on_motor1_slider_valueChanged(int value)
{
	ui->motor1_speedLabel->setText( QString::number(value) );
	if( this->ui->offsetCheckBox->isChecked() )
		return;

    if(ui->forwardModeCheckBox->isChecked() == true)
        return;

	serialSendDataToCar();
}

void MainWindow::on_motor2_slider_valueChanged(int value)
{
	ui->motor2_speedLabel->setText( QString::number(value) );
	if( this->ui->offsetCheckBox->isChecked() )
		return;

    if(ui->forwardModeCheckBox->isChecked() == true)
        return;

	serialSendDataToCar();
}

void MainWindow::on_motor3_slider_valueChanged(int value)
{
	ui->motor3_speedLabel->setText( QString::number(value) );
	if( this->ui->offsetCheckBox->isChecked() )
		return;

    if(ui->forwardModeCheckBox->isChecked() == true)
        return;

	serialSendDataToCar();
}

void MainWindow::on_motor4_slider_valueChanged(int value)
{
	ui->motor4_speedLabel->setText( QString::number(value) );
	if( this->ui->offsetCheckBox->isChecked() )
		return;

    if(ui->forwardModeCheckBox->isChecked() == true)
        return;

	serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse1_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    if(ui->forwardModeCheckBox->isChecked() == true)
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse2_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    if(ui->forwardModeCheckBox->checkState() == true)
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse3_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    //On forward mode, send data to car only after we parse all data
    //Sliders are updated one by one so we don't want to send fragmented data to car
    if(ui->forwardModeCheckBox->checkState() == true)
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse4_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    if(ui->forwardModeCheckBox->isChecked() == true)
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_minMotors_clicked()
{
    //We check this to make sure we don't send every slide changed value one by one.
    //It will cause congestion of Serial interface dure to to fast data send
    ui->forwardModeCheckBox->setChecked(true);

    ui->motor1_slider->setValue(ui->motor1_slider->minimum());
    ui->motor2_slider->setValue(ui->motor2_slider->minimum());
    ui->motor3_slider->setValue(ui->motor3_slider->minimum());
    ui->motor4_slider->setValue(ui->motor4_slider->minimum());

    //Reverse changes
    ui->forwardModeCheckBox->setChecked(false);

    //Also append new data to dataStructure.
    //On next implementations, data to Arduino will be send drectly from dataStructure and not from interface
    ///

    //Now we send all values updated at the same time
    serialSendDataToCar();
}

void MainWindow::on_pushButton_maxMotors_clicked()
{
    //We check this to make sure we don't send every slide changed value one by one.
    //It will cause congestion of Serial interface dure to to fast data send
    ui->forwardModeCheckBox->setChecked(true);

    ui->motor1_slider->setValue(ui->motor1_slider->maximum());
    ui->motor2_slider->setValue(ui->motor2_slider->maximum());
    ui->motor3_slider->setValue(ui->motor3_slider->maximum());
    ui->motor4_slider->setValue(ui->motor4_slider->maximum());

    //Reverse changes
    ui->forwardModeCheckBox->setChecked(false);

    //Also append new data to dataStructure.
    //On next implementations, data to Arduino will be send drectly from dataStructure and not from interface
    ///

    //Now we send all values updated at the same time
    serialSendDataToCar();
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
    if(ui->offsetCheckBox->isChecked() == false)
        return;

    updateSpeedGauge(value);
    ui->speedLabel->setText(QString::number(value));

    if(ui->forwardModeCheckBox->isChecked())
        return;

    serialSendDataToCar();
}

void MainWindow::on_leftRightSlider_valueChanged(int value)
{
    if(ui->offsetCheckBox->isChecked() == false)
        return;

    ui->directionLabel->setText( QString::number(value) );

    if(ui->forwardModeCheckBox->isChecked())
        return;

    serialSendDataToCar();
}

void MainWindow::on_forwardModeCheckBox_toggled(bool checked)
{
    //Disable the rest of the stuff on forward mode
    if(checked)
    {
        ui->motorsControlGroupBox->setEnabled(false);
        ui->carControlGroupBox->setEnabled(false);
    }
    else
    {
        ui->motorsControlGroupBox->setEnabled(true);
        ui->carControlGroupBox->setEnabled(true);
    }
}

void MainWindow::on_emergencyPushButton_clicked()
{
    //Send safe brake to Arduino

    //Update data structure
    for(int i=1; i<=4; i++)
        dataStructure->setMotorInfo(i, 0, DataStructure::FORWARD);
    dataStructure->setSpeed(0);
    dataStructure->setSteering(0);

    //Update GUI
    ui->motor1_slider->setValue(ui->motor1_slider->minimum());
    ui->motor2_slider->setValue(ui->motor2_slider->minimum());
    ui->motor3_slider->setValue(ui->motor3_slider->minimum());
    ui->motor4_slider->setValue(ui->motor4_slider->minimum());

    ui->pushButton_reverse1->setChecked(false);
    ui->pushButton_reverse2->setChecked(false);
    ui->pushButton_reverse3->setChecked(false);
    ui->pushButton_reverse4->setChecked(false);

    ui->speedSlider->setValue(0);
    ui->leftRightSlider->setValue(0);

    //Make sure we send brake to Arduino
    serialSendDataToCar();

}
