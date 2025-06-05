#ifndef PACKET_H
#define PACKET_H

enum packetStates {  };

class Packet 
{
private:
    int storageTime, transportTime, state;
public:
    int getStorageTime()            { return this->storageTime; }
    void setStorageTime(int n)      { this->storageTime = n; }

    int getTransportTime()          { return this->transportTime; }
    void setTransportTime(int n)    { this->transportTime = n; }
};

#endif