#pragma once
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <iostream>

class Epoller{
    friend class SocketHandler;
public:
    Epoller(bool et=true);
    ~Epoller();
    void create(int maxnum);
    void add(int fd,void *ptr,__uint32_t event);
    void del(int fd,void *ptr,__uint32_t event);
    void mod(int fd,void *ptr,__uint32_t event);
    int wait(int millisecond);
protected:
    void ctrl(int fd,void *ptr,__uint32_t event,int op);
    int m_epfd;
    int m_maxnum;;
    bool m_et;
    epoll_event * m_evs;
};

