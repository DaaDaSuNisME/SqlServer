#pragma once
#include <iostream>
#include <queue>
#include <functional>
#include <mutex>

#include "Task.h"

class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();
    void add_task(Task* task);
    int task_number();
    Task* get_Task();
private:
    std::queue< Task* > task_queue;
    std::mutex mtx_;
};
