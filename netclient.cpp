#include "netclient.h"
#include <zmq.h>
#include <cstdio>
#include<cstring>
#include <thread>
namespace superrpc
{

NetClient::NetClient()
{
    m_pServer = nullptr;
}

bool NetClient::init(std::string serverInfo,std::string clientID)
{
    void* ctx = zmq_ctx_new();
    void* server = zmq_socket(ctx, ZMQ_DEALER);
    if(zmq_connect(server, serverInfo.c_str()) != 0){
        return  false;
    }
    zmq_setsockopt (server, ZMQ_IDENTITY, clientID.c_str(), clientID.size());
    m_pServer = server;

    sendData(0,clientID.c_str(),clientID.size());
    return  true;
}

void NetClient::sendData(void* pData,int size)
{
    zmq_msg_t msg;
    zmq_msg_init_size (&msg,size);
    void *pMsg = zmq_msg_data (&msg);
    memcpy(pMsg,pData,size);

    int rc = zmq_msg_send(&msg, m_pServer, ZMQ_DONTWAIT);
    zmq_msg_close(&msg);

}

bool NetClient::sendData(int header,const char* pData,int size,std::string clientID)
{
    ZMQPack pack;
    pack.header = header;
    pack.dataSize = size;

    int sendsize= sizeof(pack.header) + sizeof(pack.dataSize) + pack.dataSize;
    zmq_msg_t msg;
    zmq_msg_init_size (&msg,sendsize);
    char *pMsg = (char*)zmq_msg_data (&msg);
    memcpy(pMsg,&pack.header,sizeof(pack.header));
    memcpy(pMsg+sizeof(pack.header),&pack.dataSize,sizeof(pack.dataSize));
    memcpy(pMsg+sizeof(pack.header)+sizeof(pack.dataSize),pData,size);
    int rc = zmq_msg_send(&msg, m_pServer, ZMQ_DONTWAIT);
    zmq_msg_close(&msg); 
}

void NetClient::clientLoop()
{
    zmq_pollitem_t items[] = {
        { m_pServer, 0, ZMQ_POLLIN, 0}
    };

    while (m_bRun)
    {
        zmq_msg_t message;
        zmq_poll(items, 1, -1);

        if(items[0].revents & ZMQ_POLLIN)
        {
            zmq_msg_init(&message);         
            int size = zmq_msg_recv(&message, m_pServer, 0);
            if(size == -1){
                continue;
            }
            
            ZMQPack pack;
            char *pData = (char*)zmq_msg_data(&message);
            memcpy(&pack.header, pData, sizeof(pack.header));
            memcpy(&pack.dataSize,pData+sizeof(pack.header),sizeof(pack.dataSize));
            pack.pData = (char*)malloc(pack.dataSize);
            memcpy(pack.pData,pData+ sizeof(pack.dataSize) + sizeof(pack.header),pack.dataSize);

            if(m_funcData){
                m_funcData(&pack,size);
            }
            free(pack.pData);
            zmq_msg_close(&message);
        }
    }
}
void NetClient::runClient()
{
    m_bRun = true;

    std::thread run([this](){
        this->clientLoop();
    });   
    run.detach();
}

void NetClient::endClient()
{
    m_bRun = false;
    sendData(0,"0",sizeof(char));  
}

bool NetServer::init(std::string serverInfo)
{
    void *ctx = zmq_ctx_new ();
    void *server = zmq_socket (ctx, ZMQ_ROUTER);
    int rc = zmq_bind (server, serverInfo.c_str());
    m_pServer = server;

    return true;
}

void NetServer::runServer()
{
    m_bRun = true;

    std::thread run([this](){
        this->serverLoop();
    });

    run.detach();
}

bool NetServer::sendData(const char* pData,int size)
{
   sendData2(1,pData,size);
}

bool NetServer::sendData2(int header,const char* pData,int size)
{
    ZMQPack pack;
    pack.header = header;
    pack.dataSize = size;

    int sendsize= sizeof(pack.header) + sizeof(pack.dataSize) + pack.dataSize;
    zmq_msg_t msg;
    zmq_msg_init_size (&msg,sendsize);
    char *pMsg = (char*)zmq_msg_data (&msg);
    memcpy(pMsg,&pack.header,sizeof(pack.header));
    memcpy(pMsg+sizeof(pack.header),&pack.dataSize,sizeof(pack.dataSize));
    memcpy(pMsg+sizeof(pack.header)+sizeof(pack.dataSize),pData,size);
    int rc = zmq_msg_send(&msg, m_pServer, ZMQ_DONTWAIT);
    zmq_msg_close(&msg); 

    return true;
}

bool NetServer::sendData(int header,const char* pData,int size,std::string clientID)
{
    auto client = m_mapClient.find(clientID);
    if(client == m_mapClient.end()){
        return false;
    }
    zmq_msg_t adress;
    zmq_msg_init(&adress);
    zmq_msg_copy(&adress,client->second);    
    zmq_msg_send (&adress, m_pServer, ZMQ_SNDMORE);
    ZMQPack pack;
    pack.header = header;
    pack.dataSize = size;

    int sendsize= sizeof(pack.header) + sizeof(pack.dataSize) + pack.dataSize;
    zmq_msg_t msg;
    zmq_msg_init_size (&msg,sendsize);
    char *pMsg = (char*)zmq_msg_data (&msg);
    memcpy(pMsg,&pack.header,sizeof(pack.header));
    memcpy(pMsg+sizeof(pack.header),&pack.dataSize,sizeof(pack.dataSize));
    memcpy(pMsg+sizeof(pack.header)+sizeof(pack.dataSize),pData,size);
    int rc = zmq_msg_send(&msg, m_pServer, ZMQ_DONTWAIT);
    zmq_msg_close(&msg); 
    zmq_msg_close(&adress);

    return true;
}

void NetServer::endServer()
{
    m_bRun = false;
    sendData2(0,"0",sizeof(char));   
}

void NetServer::serverLoop()
{
    zmq_pollitem_t items[] = {
        { m_pServer, 0, ZMQ_POLLIN, 0}
    };

    while (m_bRun)
    {
        zmq_msg_t message;
        zmq_msg_t address;
        zmq_poll(items, 1, -1);

        if(items[0].revents & ZMQ_POLLIN)
        {
            zmq_msg_init(&address);
            zmq_msg_init(&message);
            int addressSize = zmq_msg_recv(&address, m_pServer, 0);
            if(addressSize == -1){
                continue;
            }            
            int size = zmq_msg_recv(&message, m_pServer, 0);
            if(size == -1){
                continue;
            }
            
            ZMQPack pack;
            char *pData = (char*)zmq_msg_data(&message);
            memcpy(&pack.header, pData, sizeof(pack.header));
            memcpy(&pack.dataSize,pData+sizeof(pack.header),sizeof(pack.dataSize));
            pack.pData = (char*)malloc(pack.dataSize);
            memcpy(pack.pData,pData+ sizeof(pack.dataSize) + sizeof(pack.header),pack.dataSize);


            if(pack.header == 0){
                char *pData = (char*)malloc(addressSize);
                //char *pMsg = (char*)zmq_msg_data(&address);
                memcpy(pData, zmq_msg_data(&address), addressSize);
                zmq_msg_t* pNewMsg = new zmq_msg_t;
                zmq_msg_init(pNewMsg);
                zmq_msg_copy(pNewMsg,&address);
                std::string strClientid(pack.pData,pack.dataSize);
                m_mapClient[strClientid] = pNewMsg;
            }
            if(m_funcData){
                m_funcData(&pack,size);
            }
            free(pack.pData);
            zmq_msg_close(&message);
            zmq_msg_close(&address);
        }
    }
    
}

}
