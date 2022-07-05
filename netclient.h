#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <string>
#include <functional>
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
    void runServer();
    void serverLoop();

    bool sendCreateObject();
    void *m_pServer;
    std::function<void(const char* pData,int size)> m_funcData;
    bool m_bRun;
};

#endif // NETCLIENT_H
