#pragma once
#include <mutex>
#include "../Mysql/MysqlPool.h"
class Task{
public:
    Task();
    Task(void *data);
    virtual ~Task();

    void* get_data();
    void set_data(void *data);

    virtual void run()=0;
    virtual void destroy()=0;
protected:
    void * m_data;
    std::mutex m_mtx;
};