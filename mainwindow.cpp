#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QtNetwork>
#include <qbytearray.h>
#include <stdint.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    MainWindow::setWindowTitle("Xiaomi Wifi Lamp");
    ui->titleLabel->setText("Xiaomi Wifi Lamp");

    mcast_address = QHostAddress("239.255.255.250");//UDP MultiCastIP
    udp_port = 1982;//UDP port

    getIPLocal();
    initNetwork();
    detectLightBulbs();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    ui->brightnessLabelValue->setNum(value);
}

void MainWindow::getIPLocal()
{//Obtener IP local
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach (QHostAddress address, info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
//                ui->label->setText(address.toString());
            local_ip = address.toString();//Store local IP
            qDebug()<<"IP:"<<address.toString();
        }
    }
}

void MainWindow::initNetwork()
{ //Initialize the network
    udp_socket.close();
    if (false == udp_socket.bind(QHostAddress(local_ip), 0, QUdpSocket::ShareAddress))
    {
        qDebug() << "udp bind failed\n";
        return;
    }
    else
    {
        qDebug() << "udp bind success\n";
    }

    udp_socket.joinMulticastGroup(mcast_address);
    connect(&udp_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    // The data stream comes to trigger the readyRead() signal
}

void MainWindow::processPendingDatagrams()
{
    while (udp_socket.hasPendingDatagrams())
    { // Whether there is a signal to be processed
        qDebug()<<"udp receive data";
        udp_datagram_receiver.resize(udp_socket.pendingDatagramSize()); //Initialize the datagram with the size of the packet
        udp_socket.readDatagram(udp_datagram_receiver.data(), udp_datagram_receiver.size()); //Read data

        QByteArray start_str;
        QByteArray end_str;
        QByteArray rtn_str;

        //Extract bulb_ip
        start_str.clear(); end_str.clear(); rtn_str.clear();
        start_str.append("Location: yeelight://");
        end_str.append(":");
        sub_string(start_str, end_str, rtn_str);
        if(rtn_str.isEmpty() == false)
        {
            lightBulb_ip = rtn_str;
        }

        //Extract bulb_id
        start_str.clear(); end_str.clear(); rtn_str.clear();
        start_str.append("id: ");
        end_str.append("\r\n");
        sub_string(start_str, end_str, rtn_str);
        if(rtn_str.isEmpty() == false)
        {
            lightBulb_id = rtn_str;
        }

        //Filter repeatedly detected bulbs
        LightBulb bulb_tmp(lightBulb_ip.toStdString(), lightBulb_id.toStdString());
        vector<LightBulb>::iterator it = std::find(lightBulbs.begin(), lightBulbs.end(), bulb_tmp);
        if (it == lightBulbs.end())
        {
            lightBulbs.push_back(bulb_tmp);

            QStringList items;
            QString tmp;
            tmp = lightBulb_ip;
            items << tmp;
//            ui->comboBox->addItems(items);
            qDebug()<<udp_datagram_receiver.data();

        }
    }
}

int MainWindow::sub_string(QByteArray &start_str, QByteArray &end_str, QByteArray &rtn_str)
{//Extract string
    QByteArray result;
    int pos1 = -1;
    int pos2 = -1;

    result.clear();
    pos1 = udp_datagram_receiver.indexOf(start_str, 0);
    if (pos1 != -1)
    {
        result = udp_datagram_receiver.mid(pos1);
        pos1 = start_str.length();
        result = result.mid(pos1);
        pos2 = result.indexOf(end_str);
        result = result.mid(0, pos2);
    }
    rtn_str = result;

    return 0;
}

void MainWindow::detectLightBulbs()
{//"Detect"button
    QByteArray datagram =  "M-SEARCH * HTTP/1.1\r\n\
                            HOST: 239.255.255.250:1982\r\n\
                            MAN: \"ssdp:discover\"\r\n\
                            ST: wifi_bulb";

    int ret = udp_socket.writeDatagram(datagram.data(), datagram.size(), mcast_address, udp_port);
    qDebug()<<"udp write "<<ret<<" bytes";
}
