#pragma once
#include "TaskQueue.h"
#include <condition_variable>
#include <thread>
#include <atomic>
#include <algorithm>

class ThreadPool {
public:
    ThreadPool();
    ThreadPool(int ,int );
    ~ThreadPool();
    void init(int, int);
    void add_task(Task*);
    int get_alive_num();
    int get_busy_num();
    void join();
private:
    static void work(ThreadPool*);
    static void manager(ThreadPool*);
private:
    TaskQueue* m_task_queue;
    std::vector<std::thread> m_threads;
    std::thread m_manager;
    std::condition_variable m_not_empty;
    std::mutex m_lock;
    int m_alive_num;
    int m_busy_num;
    int m_exit_num;
    int m_max_num;
    int m_min_num;
    bool m_terminal;
};
