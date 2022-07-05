#include "superrpc.h"
#include "netclient.h"
#include "rpcobjectmanager.h"
#include "thread"
namespace superrpc
{

    std::shared_ptr<NetServer> g_netServer = nullptr;
    bool InitServer(std::string serverInfo)
    {
        auto pServer = std::make_shared<NetServer>();
        if(!pServer->init(serverInfo)){
            return false;
        }
        

        g_netServer = pServer;
        return true;
    }
};