#include "ThreadPool.h"

ThreadPool::ThreadPool():m_min_num(0), m_max_num(0),m_terminal(false) {

}

ThreadPool::ThreadPool(int min, int max) :m_min_num(min), m_max_num(max),m_terminal(false) {
    this->init(min,max);
}

ThreadPool:: ~ThreadPool() {
    this->m_terminal = true;
    if (this->m_manager.joinable()) this->m_manager.join();
    this->m_not_empty.notify_all();
    for (int i = 0; i < this->m_max_num; ++i) {
        if (this->m_threads[i].joinable()) this->m_threads[i].join();
    }
    if (this->m_task_queue) delete this->m_task_queue;
}

void ThreadPool::init(int min_num,int max_num){
    this->m_max_num=max_num;
    this->m_min_num=min_num;
    this->m_task_queue = new TaskQueue;
    try {
        this->m_threads = std::vector<std::thread>(max_num);
    }
    catch (...) {
        std::cout << "heap offset error" << std::endl;
        return;
    }
    for (int i = 0; i < this->m_min_num; ++i) {
        this->m_threads[i] = std::thread(ThreadPool::work, this);
        std::cout << "Create new Thread,ID:" << m_threads[i].get_id() << std::endl;
    }
    this->m_manager = std::thread(ThreadPool::manager, this);
}

void ThreadPool::add_task(Task* task) {
    if (this->m_terminal) return;
    this->m_task_queue->add_task(task);
    this->m_not_empty.notify_one();
}

int ThreadPool::get_alive_num() {
    std::unique_lock<std::mutex> lock(this->m_lock);
    int res_num = this->m_alive_num;
    lock.unlock();
    return res_num;
}

int ThreadPool::get_busy_num() {
    std::unique_lock<std::mutex> lock(this->m_lock);
    int res_num = this->m_busy_num;
    lock.unlock();
    return res_num;
}

void ThreadPool::join() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (this->get_busy_num());
}

void ThreadPool::work(ThreadPool* pool) {
    std::unique_lock<std::mutex> lock(pool->m_lock);
    lock.unlock();
    auto this_id = std::this_thread::get_id();
    while (true) {
        lock.lock();
        while (!pool->m_terminal && pool->m_task_queue->task_number() == 0) {
            pool->m_not_empty.wait(lock);
            //maybe judge this thread neet to be exited ,but cant out the while
            if (pool->m_exit_num > 0) {
                --pool->m_exit_num;
                if (pool->m_alive_num > pool->m_min_num) {
                    --pool->m_alive_num;
                    lock.unlock();
                    std::cout << " thread ID:" << this_id << "  is exinting for mexit_num" << std::endl;
                    return;
                }
            }
        }
        if (pool->m_terminal) {
            std::cout << " thread ID:" << this_id << "  is exinting for m_terminal" << std::endl;
            return;
        }
        ++pool->m_busy_num;
        Task* task = pool->m_task_queue->get_Task();
        lock.unlock();

        task->run();
        task->destroy();//dont explicit call delete

        lock.lock();
        --pool->m_busy_num;
        lock.unlock();
    }
}

void ThreadPool::manager(ThreadPool* pool) {
    std::unique_lock<std::mutex> lock(pool->m_lock);
    lock.unlock();
    while (!pool->m_terminal) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        lock.lock();
        int busy_num = pool->m_busy_num;
        int alive_num = pool->m_alive_num;
        int task_num = pool->m_task_queue->task_number();
        lock.unlock();
        const int NUMBER = 2;
        if (alive_num < pool->m_max_num && alive_num-busy_num < task_num) {
            int count = 0;
            for (int i = 0; i < pool->m_max_num && count < std::max(task_num-alive_num,NUMBER); ++i) {
                if (!pool->m_threads[i].joinable()) {
                    pool->m_threads[i] = std::thread(ThreadPool::work, pool);
                    std::cout << "Create new thread ID:" << pool->m_threads[i].get_id() << std::endl;
                    ++count;
                    lock.lock();
                    ++pool->m_alive_num;
                    lock.unlock();
                }

            }
        }
        if (2 * busy_num<alive_num && alive_num>pool->m_min_num) {
            lock.lock();
            pool->m_exit_num += NUMBER;
            lock.unlock();
            for (int i = 0; i < NUMBER; ++i) {
                pool->m_not_empty.notify_one();
            }
        }
    }
}