#include "TaskQueue.h"

TaskQueue::TaskQueue(){

}

TaskQueue::~TaskQueue(){

}

void  TaskQueue::add_task(Task* task) {
    std::lock_guard<std::mutex> locker(this->mtx_);
    this->task_queue.push(task);
}

int TaskQueue::task_number() {
    int task_num = this->task_queue.size();
    return task_num;
}

Task* TaskQueue::get_Task() {
    std::unique_lock<std::mutex> lock(this->mtx_);
    if (this->task_number() == 0) return nullptr;
    Task* res_task = this->task_queue.front();
    this->task_queue.pop();
    lock.unlock();
    return res_task;
}