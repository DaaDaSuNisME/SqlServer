#pragma once
#include "../Thread/Task.h"
#include "../Socket/Socket.h"
#include "WorkTask.h"
#include "EchoTask.h"

class TaskFactory{
public:
    static Task * produce(Socket* socket){
        //return new WorkTask(socket);
        return new WorkTask(socket);
    }
};

