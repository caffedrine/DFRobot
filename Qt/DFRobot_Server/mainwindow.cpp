#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QList>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    pSerialPort (0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    viewGauge = new QQuickView();
    containerGauge = QWidget::createWindowContainer(viewGauge, this);
    containerGauge->setMinimumSize(160, 160);
    containerGauge->setMaximumSize(160, 160);
    containerGauge->setFocusPolicy(Qt::TabFocus);
    viewGauge->setSource(QUrl("../CircularGauge.qml"));
    ui->verticalLayout->addWidget(containerGauge);
}

MainWindow::~MainWindow()
{
    delete ui;

    if(pSerialPort)
        delete pSerialPort;
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

void MainWindow::on_pushButton_connectArduino_clicked()
{
    //QMessageBox::information(this, tr("Important message"), tr("This is all I want to say!"));
	/*
    if(pSerialPort)
        delete pSerialPort;

    pSerialPort = 0;

    if(ui->comboBox_serialSlots->currentText().isEmpty())
    {
        qDebug() << "FAILED: No serial port selected!";
        return;
    }

    //Creating a new instance
    pSerialPort = new QSerialPort();

    //Setting connection params
    pSerialPort->setPortName( ui->comboBox_serialSlots->currentText() );
    pSerialPort->setBaudRate(QSerialPort::Baud19200);

    //connection params signals and slots
    connect(pSerialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(serialConnectionChanged(QSerialPort::SerialPortError)));
    connect( pSerialPort, SIGNAL(readyRead()), this, SLOT(serialDataReceivingSlot()) );

    //Start RW Session
    pSerialPort->open( QIODevice::ReadWrite );

    if(!pSerialPort->isOpen())
    {
        qDebug() << "FAILED: Can't connect to serial port!";
    }
    else
    {
        qDebug() << "SUCCESS: CONNECTED";
    }
	*/

	//Getting portname and baudrate
	QString portName = ui->comboBox_serialSlots->currentText();
	SerialConn::BoudRate baud = SerialConn::BoudRate::Baud19200;

	if(serialPort)
	{
		delete serialPort;
		serialPort = Q_NULLPTR;
	}

	serialPort = new SerialConn();

	//connecting serial signals
	connect(serialPort, SIGNAL(connectionStatusChanged(bool)), this, SLOT(setSerialPortStatus(bool)));
	//connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialDataReceivingSlot()));

	if(serialPort->connect(portName, baud))
	{
		qDebug() << "SUCCESS: You're now connected to Arduino!";
	}
	else
	{
		qDebug() << "FAILED: " << serialPort->getLastError();
		delete serialPort;
		serialPort = Q_NULLPTR;
	}
}

void MainWindow::serialDataReceivingSlot()
{
    // Read data
    static QByteArray byteArray;
    byteArray += pSerialPort->readAll();

    //we want to read all message not only chunks
    if(!QString(byteArray).contains("\n"))
        return;

    //sanitize data
    QString data = QString( byteArray ).remove("\r").remove("\n");
    byteArray.clear();

    // Print data
    qDebug() << "RECV: " << data;

    //Send it to visual console too
    ui->arduinoConnectionConsole->appendPlainText(data);
}

bool MainWindow::serialWrite(QString str)
{
    qint64 currMillis = QDateTime::currentMSecsSinceEpoch();
    if(currMillis - prevMillis < 50)
    {
        return false;
    }
    prevMillis = currMillis;

	if(serialPort == Q_NULLPTR)
	{
		qDebug() << "FAILED: Please connect to Arduino board first!";
		return false;
	}

	qint64 bytesWritten = serialPort->write(str);
	if( bytesWritten > 0)
	{
		QString dbgStr = "SUCCESS: SEND (" + QString::number(bytesWritten) + "bytes): " + str;

		qDebug() << dbgStr;
	}
	else
	{
		qDebug() << "FAILED: " << serialPort->getLastError();
	}

	/*
    if(pSerialPort == Q_NULLPTR)
    {
        qDebug() << "FAILED: Please connect to Arduino board first!";
        return false;
    }

    if( pSerialPort->isOpen() )
    {
        QByteArray writeData = str.toUtf8();
        pSerialPort->write( writeData );
        qDebug() << "SEND: " << str;
        return true;
    }
    else
    {
        qDebug() << "FAILED: Cannot write on serial port...";
        return false;
    }
	*/
}

void MainWindow::on_pushButton_updateList_clicked()
{
    //Get serial ports info
    QSerialPortInfo *portsInfo = new QSerialPortInfo;

    //Fetching available ports into a list
    QList<QSerialPortInfo> serialPorts = portsInfo->availablePorts();

    //Cleaning combolist
    ui->comboBox_serialSlots->clear();

    //Filling combo list
    foreach (QSerialPortInfo serialPort, serialPorts)
    {
        ui->comboBox_serialSlots->addItem(serialPort.portName());
    }

    //clearing pointer we just created
    delete portsInfo;

    //Send some debug info
    qDebug() << "SUCCESS: Serial ports list updated!";
}

void MainWindow::on_motor1_slider_valueChanged(int value)
{
    ui->motor1_speedLabel->setText( QString::number(value) );
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_motor2_slider_valueChanged(int value)
{
    ui->motor2_speedLabel->setText( QString::number(value) );
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_motor3_slider_valueChanged(int value)
{
    ui->motor3_speedLabel->setText( QString::number(value) );
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_motor4_slider_valueChanged(int value)
{
    ui->motor4_speedLabel->setText( QString::number(value) );
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_disconnectArduino_clicked()
{
	/*
    if(this->pSerialPort != Q_NULLPTR)
    {
        if(pSerialPort->isOpen())
            this->pSerialPort->close();
		delete pSerialPort;
    }
    else
    {
        qDebug() << "FAILED: You're not connected to Arduino!";
        return;
    }

    this->setSerialPortStatus(0);
    qDebug() << "SUCCESS: DISCONNECTED";
	*/

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

void MainWindow::serialConnectionChanged(QSerialPort::SerialPortError errNo)
{

	(errNo == 0) ? this->setSerialPortStatus(1) : this->setSerialPortStatus(0);
}

void MainWindow::on_startServerButton_clicked()
{
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
    if(server)
        delete server;

    server = new TcpServer(this);
    server->setPort( QString(ui->rcConnectionPortTextBox->text()).toInt() );

    if(server->startServer());
    {
        ui->rcConnectionServerStatus->setText("STARTED");
    }
}

void MainWindow::processRecvData(QString data)
{
    ui->rcConnectionConsoleTextBox->appendPlainText(data);

    if( !data.contains("[") || !data.contains("]") || !data.contains("|") )
        return;

    QStringList motors = data.split(",");
	for(int i=0; i<=5; i++)
    {
        QString motor = motors[i].remove("[").remove("]");

        int speed = motor.split("|")[0].toInt();
        int direction = motor.split("|")[1].toInt();

        switch(i)
        {
            case 0:
            {
                ui->motor1_slider->setValue(speed);
                ui->pushButton_reverse1->setChecked( direction ? 0 : 1 );
                break;
            }

            case 1:
            {
                ui->motor2_slider->setValue(speed);
                ui->pushButton_reverse2->setChecked( direction ? 0 : 1 );
                break;
            }

            case 2:
            {
                ui->motor3_slider->setValue(speed);
                ui->pushButton_reverse3->setChecked( direction ? 0 : 1 );
                break;
            }

            case 3:
            {
                ui->motor4_slider->setValue(speed);
                ui->pushButton_reverse4->setChecked( direction ? 0 : 1 );
                break;
            }

			case 4:
			{
				ui->speedSlider->setValue(direction);
				break;
			}

			case 5:
			{
				ui->leftRightSlider->setValue(direction);
				break;
			}
        }
    }
}

void MainWindow::on_stopServerButton_clicked()
{
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

void MainWindow::clientConnectionChanged(bool connection)
{
    ui->rcConnectionClientStatus->setText( connection ? "CONNECTED" : "NOT CONNECTED" );
}

void MainWindow::serialSendDataToCar()
{
    QString msg = ""; // [M1,0,255] - motor, direction, speed

    if(ui->offsetCheckBox->isChecked() == false)
    {
        msg += "[M1,";
        msg += ui->pushButton_reverse1->isChecked() ? "1" : "0";
        msg += ",";
        msg += QString::number(ui->motor1_slider->value());
        msg += "]-";

        msg += "[M2,";
        msg += ui->pushButton_reverse2->isChecked() ? "1" : "0";
        msg += ",";
        msg += QString::number(ui->motor2_slider->value());
		msg += "]-";

        msg += "[M3,";
        msg += ui->pushButton_reverse3->isChecked() ? "1" : "0";
        msg += ",";
        msg += QString::number(ui->motor3_slider->value());
        msg += "]-";

        msg += "[M4,";
        msg += ui->pushButton_reverse4->isChecked() ? "1" : "0";
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
        int overVal = 0;

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
	//qDebug() << msg << "\n";
	serialWrite(msg);
}

void MainWindow::on_pushButton_reverse1_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse2_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse3_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_reverse4_clicked()
{
    if( this->ui->offsetCheckBox->isChecked() )
        return;

    serialSendDataToCar();
}

void MainWindow::on_pushButton_minMotors_clicked()
{
    ui->motor1_slider->setValue(ui->motor1_slider->minimum());
    ui->motor2_slider->setValue(ui->motor2_slider->minimum());
    ui->motor3_slider->setValue(ui->motor3_slider->minimum()); QThread::msleep(50); //because I can!!!
    ui->motor4_slider->setValue(ui->motor4_slider->minimum());

}

void MainWindow::on_pushButton_maxMotors_clicked()
{
    ui->motor1_slider->setValue(ui->motor1_slider->maximum());
    ui->motor2_slider->setValue(ui->motor2_slider->maximum());
    ui->motor3_slider->setValue(ui->motor3_slider->maximum()); QThread::msleep(50);
    ui->motor4_slider->setValue(ui->motor4_slider->maximum());
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
    if(ui->offsetCheckBox->isChecked() == false)
        return;

    updateSpeedGauge(value);
    ui->speedLabel->setText(QString::number(value));

    serialSendDataToCar();
}

void MainWindow::on_leftRightSlider_valueChanged(int value)
{
    if(ui->offsetCheckBox->isChecked() == false)
        return;

    ui->directionLabel->setText( QString::number(value) );

    serialSendDataToCar();
}
