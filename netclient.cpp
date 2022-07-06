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

bool NetClient::init(std::string serverInfo)
{
    void* ctx = zmq_ctx_new();
    void* server = zmq_socket(ctx, ZMQ_DEALER);
    if(zmq_connect(server, serverInfo.c_str()) != 0){
        return  false;
    }
    m_pServer = server;

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

void NetClient::clientLoop()
{
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    int size = zmq_msg_recv(&msg, m_pServer, ZMQ_DONTWAIT);
    if(size == -1)
        return ;

    char *string = (char*)malloc(size + 1);
    memcpy(string, zmq_msg_data(&msg), size);

    zmq_msg_close(&msg);
    string[size] = 0;
    return ;
}


bool NetServer::init(std::string serverInfo)
{
    void *ctx = zmq_ctx_new ();
    void *server = zmq_socket (ctx, ZMQ_ROUTER);
    int rc = zmq_bind (server, serverInfo.c_str());
    m_pServer = server;
}

void NetServer::runServer()
{
    m_bRun = true;

    std::thread run([this](){
        this->serverLoop();
    });
}

bool NetServer::sendCreateObject(std::string strObjectName,std::int64_t objectID)
{

}

void NetServer::endServer()
{
    m_bRun = false;
    int size= 1;
    zmq_msg_t msg;
    zmq_msg_init_size (&msg,size);
    void *pMsg = zmq_msg_data (&msg);
    memcpy(pMsg,"0",size);

    int rc = zmq_msg_send(&msg, m_pServer, 0);
    zmq_msg_close(&msg);    
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
            
            char *pData = (char*)malloc(size);
            memcpy(pData, zmq_msg_data(&message), size);
            ZMQPack *pPack = (ZMQPack*)pData;
            if(pPack->header == 0){
                char *pData = (char*)malloc(addressSize);
                memcpy(pData, zmq_msg_data(&message), size);
                zmq_msg_t* pNewMsg = new zmq_msg_t;
                zmq_msg_init(pNewMsg);
                zmq_msg_copy(pNewMsg,&address);
                m_mapClient[pData] = pNewMsg;
            }
            if(m_funcData){
                m_funcData(pData,size);
            }
            free(pData);
            zmq_msg_close(&message);
            zmq_msg_close(&address);
        }
    }
    
}

}
