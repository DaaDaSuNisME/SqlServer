#pragma once
#include "../Thread/Task.h"
#include "../Socket/Socket.h"
#include "../Tools/Singleton.h"

//used to test socket commucation
class EchoTask : public Task{
public:
    EchoTask(Socket *socket);
    virtual ~EchoTask();

    virtual void run();
    virtual void destroy();
};