#include "netclient.h"
#include <zmq.h>
#include <cstdio>
#include<cstring>
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
    void *server = zmq_socket (ctx, ZMQ_DEALER);
    int rc = zmq_bind (server, serverInfo.c_str());

}

void NetServer::serverLoop()
{

}
