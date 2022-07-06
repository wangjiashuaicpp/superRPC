#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <string>
#include <functional>
#include <unordered_map>
struct zmq_msg_t;
namespace superrpc
{
struct ZMQPack
{
    int header;
    char *pData;
    int dataSize;

    int calcSize()
    {
        int size = sizeof(header) + dataSize + sizeof(dataSize);
        return size;
    }
};

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
    void endServer();

    bool sendCreateObject(std::string strObjectName,std::int64_t objectID);
    void *m_pServer;
    std::function<void(const char* pData,int size)> m_funcData;
    std::unordered_map<std::string,zmq_msg_t*> m_mapClient;
    bool m_bRun;
};
}
#endif // NETCLIENT_H
