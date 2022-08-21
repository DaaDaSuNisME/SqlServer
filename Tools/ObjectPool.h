#pragma once
#include <list>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include <mutex>

template <typename T>
class ObjectPool{
public:
    ObjectPool();
    ~ObjectPool();

    void init(int num);
    T* allocate();
    void release(T *p);
private:
    std::list<T *> m_poll;
    std::mutex m_mtx;
};

template <typename T>
ObjectPool<T>::ObjectPool(){

}

template <typename T>
ObjectPool<T>::~ObjectPool(){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    for(auto iter=this->m_poll.begin();iter!=this->m_poll.end();++iter){
        T* node=*iter;
        if( node ){
            delete node;
        }
    }
    this->m_poll.clear();
}

template <typename T>
void ObjectPool<T>::init(int num){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    for(int i=0;i<num;++i){
        this->m_poll.push_back(new T());
    }
}

template <typename T>
T* ObjectPool<T>::allocate(){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    auto res=this->m_poll.front();
    this->m_poll.pop_front();
    return res;
}

template <typename T>
void ObjectPool<T>::release(T *p){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    this->m_poll.push_back(p);
}