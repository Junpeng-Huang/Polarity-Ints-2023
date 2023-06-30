#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Timer.h>
#include <defines.h>
#include <pins.h>
#include "SoftwareSerial.h"

class Bluetooth
{
    private:
        void send();
        void receive();
        void decideRole();
        Timer sendTimer = Timer(BT_SEND_TIMER);
        Timer connectedTimer = Timer(CONNECTED_TIMER);
        uint16_t sameRole = 0;
        
    public:
        void init();
        void update(float ballStrSurge); 
        BluetoothData thisData = {0, -1};
        BluetoothData otherData = {0, -1};
        Timer switchTimer = Timer(BT_SWITCH_TIMER);
        bool isConnected = false;
        bool isSwitching = false;

};


#endif