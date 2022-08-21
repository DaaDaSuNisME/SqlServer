#pragma once
#include <mutex>

#include "Epoller.h"
#include "ServerSocket.h"
#include "../Tools/ObjectPool.h"
#include "../Tools/Singleton.h"
#include "../Task/TaskFactory.h"// SocketHandler include TaskFactory
#include "../Task/EchoTask.h"
#include "../Task/TaskDispacher.h"
#include "../Thread/ThreadPool.h"

class SocketHandler{
public:
    SocketHandler();
    ~SocketHandler();
    void listen(const std::string & ip,int port);
    void attach(Socket *socket);
    void detach(Socket *socket);
    void remove(Socket *socket);
    void handle(int maxconnection,int timewait);
private:
    Epoller * m_epoller;
    Socket * m_listensocket;
    ObjectPool<Socket> m_socketpool;
    std::mutex m_mtx;
};