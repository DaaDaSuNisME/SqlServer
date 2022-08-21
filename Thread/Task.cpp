#include "Task.h"

Task::Task():m_data(nullptr){

}

Task::Task(void *data):m_data(data){

}

Task:: ~Task(){

}

void* Task::get_data(){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    return this->m_data;
}

void Task::set_data(void *data){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    this->m_data=data;
}