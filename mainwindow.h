#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QtNetwork>
#include <stdint.h>
#include "lightbulb.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    int sub_string(QByteArray &start_str, QByteArray &end_str, QByteArray &rtn_str);

private slots:
    void on_brightnessSlider_valueChanged(int value);

    void processPendingDatagrams();

private:
    Ui::MainWindow *ui;

    QHostAddress mcast_address;
    QString local_ip;
    QString lightBulb_ip;
    QString lightBulb_id;

    vector<LightBulb> lightBulbs;


    QUdpSocket udp_socket;
    uint16_t udp_port;
    QByteArray udp_datagram_receiver;

    void getIPLocal();
    void initNetwork();
    void detectLightBulbs();
};
#endif // MAINWINDOW_H
