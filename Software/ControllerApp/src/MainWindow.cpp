/**
 * @file MainWindow.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "config.h"
#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSize>
#include <QLabel>
#include <QByteArray>
#include <QDataStream>
#include <QString>
#include <QStringListModel>


MainWindow::MainWindow()
{
    qDebug() << "Creating MainWindow";

    m_midDial = new QDial(this);
    m_bassDial = new QDial(this);
    m_trebleDial = new QDial(this);
    m_volumeSlider = new QSlider(this);

    connect(m_midDial, SIGNAL(sliderMoved(int)), this, SLOT(OnMidMoved(int)));
    connect(m_bassDial, SIGNAL(sliderMoved(int)), this, SLOT(OnBassMoved(int)));
    connect(m_trebleDial, SIGNAL(sliderMoved(int)), this, SLOT(OnTrebleMoved(int)));
    connect(m_volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnVolMoved(int)));


    QPointer<QLabel> midLabel = new QLabel("Mid");
    midLabel->setAlignment(Qt::AlignHCenter);
    QPointer<QLabel> bassLabel = new QLabel("Bass");
    bassLabel->setAlignment(Qt::AlignHCenter);
    QPointer<QLabel> trebleLabel = new QLabel("Treble");
    trebleLabel->setAlignment(Qt::AlignHCenter);
    QPointer<QLabel> volumeLabel = new QLabel("Vol");
    volumeLabel->setAlignment(Qt::AlignHCenter);

    QSize dialSize(50, 50);
    m_midDial->setFixedSize(dialSize);
    m_bassDial->setFixedSize(dialSize);
    m_trebleDial->setFixedSize(dialSize);

    QPointer<QGroupBox> controlsBox = new QGroupBox("Controls", this);
    controlsBox->setAlignment(Qt::AlignHCenter);
    QPointer<QGridLayout> controlsLayout = new QGridLayout(controlsBox);

    controlsLayout->addWidget(m_trebleDial, 0, 0);
    controlsLayout->addWidget(trebleLabel, 1, 0);
    controlsLayout->addWidget(m_midDial, 2, 0);
    controlsLayout->addWidget(midLabel, 3, 0);
    controlsLayout->addWidget(m_bassDial, 4, 0);
    controlsLayout->addWidget(bassLabel, 5, 0);
    controlsLayout->addWidget(m_volumeSlider, 0, 1, 5, 1);
    controlsLayout->addWidget(volumeLabel, 5, 1);

    controlsBox->setLayout(controlsLayout);

    m_connectButton = new QPushButton("Connect", this);
    connect(m_connectButton, SIGNAL(clicked()), this, SLOT(OnConnectClicked()));

    m_listView = new QListView(this);
    m_listModel = new QStringListModel(this);
    m_listModel->setStringList(m_list);
    m_listView->setModel(m_listModel);

    QPointer<QGroupBox> connectionBox = new QGroupBox(this);
    connectionBox->setAlignment(Qt::AlignHCenter);
    QPointer<QVBoxLayout> connectionLayout = new QVBoxLayout(connectionBox);
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addWidget(m_listView);

    connectionBox->setLayout(connectionLayout);


    QPointer<QHBoxLayout> mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addWidget(controlsBox);
    mainLayout->addWidget(connectionBox);
    mainLayout->addStretch(1);

    this->setLayout(mainLayout);

    /* setup udp socket */
    m_socket = new QUdpSocket(this);
    if(!m_socket->bind(QHostAddress::AnyIPv4, DISCOVERY_PORT, QUdpSocket::ShareAddress))
    {
        qDebug() << "ERROR: could not bind udp socket";
    }
    else
    {

        if(!m_socket->joinMulticastGroup(QHostAddress(MCAST_ADDR)))
        {
            qDebug() << "ERROR: could not join multicast group";
        }
    }
    
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    
}

void MainWindow::OnMidMoved(int val)
{
    QByteArray value, data;
    value.setNum(val);

    data.append("M");
    data.append(value);

    m_socket->writeDatagram(data, QHostAddress(MCAST_ADDR), CONTROL_PORT);
}

void MainWindow::OnBassMoved(int val)
{
    QByteArray value, data;
    value.setNum(val);

    data.append("B");
    data.append(value);

    m_socket->writeDatagram(data, QHostAddress(MCAST_ADDR), CONTROL_PORT);
}

void MainWindow::OnTrebleMoved(int val)
{
    QByteArray value, data;
    value.setNum(val);

    data.append("T");
    data.append(value);

    m_socket->writeDatagram(data, QHostAddress(MCAST_ADDR), CONTROL_PORT);
}

void MainWindow::OnVolMoved(int val)
{
    QByteArray value, data;
    value.setNum(val);

    data.append("V");
    data.append(value);

    m_socket->writeDatagram(data, QHostAddress(MCAST_ADDR), CONTROL_PORT);
}

void MainWindow::OnConnectClicked()
{
    QByteArray data;
    data.append(1);

    m_socket->writeDatagram(data, QHostAddress(MCAST_ADDR), DISCOVERY_PORT);
}

void MainWindow::readyRead()
{
    QByteArray buffer;
    buffer.resize(m_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    m_socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    if (senderPort == CONTROL_PORT)
    {
        switch(buffer.operator[](0))
        {
            case 'M':
                buffer.remove(0, 1);
                qDebug() << "MID REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.operator[](0) << endl;
                break;
            case 'B':
                qDebug() << "BASS REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.data() << endl;
                break;
            case 'T':
                qDebug() << "TREBLE REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.data() << endl;
                break;
            case 'V':
                qDebug() << "VOLUME REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.data() << endl;
                break;
        }
    }
    else if (senderPort == DISCOVERY_PORT)
    {
        qDebug() << "discover";
        if (!inRegistry(buffer.operator[](0)))
        {
            QString listStr = "Wifi-Hifi Speaker ";
            listStr.append(QString::number(buffer.operator[](0)));
            m_list << listStr;
            m_listModel->setStringList(m_list);
        }
    }

}

bool MainWindow::inRegistry(char id)
{
    for (int count = 0; count < m_registry.size(); count++)
    {
        if (m_registry.at(count) == id) return true;
    }
    m_registry.append(id);
    return false;

}
