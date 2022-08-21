#include "TaskDispacher.h"


TaskDispacher::TaskDispacher(){
    this->m_tpoll=Singleton<ThreadPool>::instance();
}

TaskDispacher::~TaskDispacher(){

}

void TaskDispacher::init(int min_num,int max_num){// create threadpool
    this->m_tpoll->init(min_num,max_num);
}

void TaskDispacher::assign(Task *task){// add task to threadpool
    this->m_tpoll->add_task(task);
} 