#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include <string>

using namespace std;

class LightBulb
{
private:
    string id;
    int port;
    string ip;

    int brightness;
    int colorTemperature;

public:
    LightBulb() {
        id.clear();
        ip.clear();
        port = 55443;
        brightness = 100;
        colorTemperature = 2700;
    }

    LightBulb(string ip, string id) {
        this->id = id;
        this->ip = ip;
        port = 55443;
        brightness = 100;
        colorTemperature = 2700;
    }

    LightBulb operator = (const LightBulb &x) {
        LightBulb lb;
        lb.id = x.id;
        lb.ip = x.ip;
        lb.port = x.port;
        lb.brightness = 100;
        lb.colorTemperature = 2700;
        return lb;
    }

    bool operator == (const LightBulb &x) {
        return (this->ip == x.ip) && (this->port == x.port) && (this->id == x.id);
    }


    string getIP() {
        return ip;
    }

    string getID() {
        return id;
    }

    int getPort() {
        return port;
    }

    int getBrightness() {
        return brightness;
    }
    void setBrightness(int value) {
        brightness = value;
    }

    int getColor() {
        return colorTemperature;
    }
    void setColor(int value) {
        colorTemperature = value;
    }
};

#endif // LIGHTBULB_H
