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

};

class NetClient
{
public:
    NetClient();

    bool init(std::string serverInfo,std::string clientID);
    void sendData(void* pData,int size);
    bool sendData(int header,const char* pData,int size);
    void clientLoop();
    void runClient();
    void endClient();    
    void setFunCall(std::function<void(const ZMQPack* pData,int size)> call){m_funcData = call;};
    std::function<void(const ZMQPack* pData,int size)> m_funcData;
    void *m_pServer;
    bool m_bRun;
};

class NetServer
{
public:
    bool init(std::string serverInfo);
    void runServer();
    void serverLoop();
    void endServer();

    bool sendData(const char* pData,int size);
    bool sendData(int header,const char* pData,int size);
    bool sendData(int header,const char* pData,int size,std::string clientID);
    void setFunCall(std::function<void(const ZMQPack* pData,int size)> call){m_funcData = call;};
    void *m_pServer;
    std::function<void(const ZMQPack* pData,int size)> m_funcData;
    std::unordered_map<std::string,zmq_msg_t*> m_mapClient;
    bool m_bRun;
};
}
#endif // NETCLIENT_H
