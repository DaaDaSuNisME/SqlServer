#pragma once
#include "../Thread/Task.h"
#include "../Socket/Socket.h"
#include "../Tools/Singleton.h"

#define QUERY_FLAG 0
#define UPDATE_FLAG 1

struct MsgHead {//Message head
    char flag[8];
    uint32_t cmd;
    uint32_t len;
};

const uint32_t recv_buff_size = 1024;

class WorkTask : public Task{
public:
    WorkTask(Socket *socket);
    virtual ~WorkTask();
    virtual void run();
    virtual void destroy();
};