#pragma once
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

class Socket{
    friend class SocketHandler;
public:
    Socket();
    Socket(const std::string &ip,int port);
    virtual ~Socket();

    bool bind(const std::string &ip,int port);
    bool listen(int backlog);
    bool connect(const std::string &ip,int port);
    bool close();

    int accept();
    int recv(char * buf,int len);
    int send(const char * buf,int len);

    bool set_non_blocking();
    bool set_send_buffer(int size);
    bool set_recv_buffer(int size);
    bool set_linger(bool active, int seconds);
    bool set_keep_alive();
    bool set_reuse_addr();
    bool set_reuse_port();

protected:
    int m_sockfd;
    std::string m_ip;
    int m_port;
};

