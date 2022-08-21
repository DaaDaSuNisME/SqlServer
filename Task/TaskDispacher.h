#pragma once
#include "../Thread/Task.h"
#include <thread>
#include <list>
#include "../Thread/ThreadPool.h"
#include "../Tools/Singleton.h"

//singleclass : create ThreadPool and assign Task
class TaskDispacher{
public:
    TaskDispacher();
    ~TaskDispacher();
    void init(int min_num,int max_num);// create threadpool
    void assign(Task *task); // add task to threadpool
private:
    ThreadPool * m_tpoll;
};