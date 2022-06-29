#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <string>
class NetClient
{
public:
    NetClient();

    bool init(std::string serverInfo);
    void sendData(void* pData,int size);
    void clientLoop();

    void *m_pServer;
};

class NetServer
{
public:
    bool init(std::string serverInfo);
    void serverLoop();

    bool sendCreateObject();
    void *m_pServer;
};

#endif // NETCLIENT_H
