#include "Epoller.h"

Epoller::Epoller(bool et):m_et(et),m_epfd(0),m_evs(nullptr),m_maxnum(0){

}

Epoller::~Epoller(){
    if(this->m_epfd){
        close(this->m_epfd);
        this->m_epfd=0;
    }
    if(this->m_evs){
        delete[] this->m_evs;
        this->m_evs=nullptr;
    }
}

void Epoller::create(int maxnum){
    this->m_maxnum=maxnum;
    this->m_epfd=epoll_create(maxnum+1);//have two function, epoll_create1(int flag)
    if(  this->m_epfd< 0){
        std::cout<<"m_epfd:"<<this->m_epfd<<std::endl;
        perror("Epoller create error:");
        return ;
    }
    if( this->m_evs){
        delete[] this->m_evs;
        this->m_evs=nullptr;
    }
    this->m_evs=new epoll_event[maxnum+1];
}

void Epoller::add(int fd,void *ptr,__uint32_t event){
    ctrl(fd,ptr,event,EPOLL_CTL_ADD);
}

void Epoller::del(int fd,void *ptr,__uint32_t event){
    ctrl(fd,ptr,event,EPOLL_CTL_DEL);
}

void Epoller::mod(int fd,void *ptr,__uint32_t event){
    ctrl(fd,ptr,event,EPOLL_CTL_MOD);
}

int Epoller::wait(int millisecond){
    return epoll_wait(this->m_epfd,this->m_evs,this->m_maxnum+1,millisecond);
}

void Epoller::ctrl(int fd,void *ptr,__uint32_t event,int op){
    epoll_event ev;
    ev.data.ptr=ptr;
    if(this->m_et){
        ev.events=event | EPOLLET;
    }else ev.events=event;
    int ret=epoll_ctl(this->m_epfd,op,fd,&ev);
    if(ret==-1){
        perror("poller::ctrl error");
    }
}