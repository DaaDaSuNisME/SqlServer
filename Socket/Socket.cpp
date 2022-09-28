#include "Socket.h"

Socket::Socket():m_sockfd(0),m_ip(""),m_port(0){

}

Socket::Socket(const std::string &ip,int port):m_sockfd(0),m_ip(ip),m_port(port){
    
}

Socket:: ~Socket(){
    this->close();
}

bool Socket::bind(const std::string &ip,int port){
    sockaddr_in caddr;
    memset(&caddr, 0, sizeof(caddr));
    caddr.sin_family=AF_INET;
    if(ip!=""){
        caddr.sin_addr.s_addr=inet_addr(ip.c_str());
    }else{
        caddr.sin_addr.s_addr=INADDR_ANY;
    }
    caddr.sin_port=htons(port);
    if(::bind(this->m_sockfd,(sockaddr*)&caddr,sizeof(caddr)) < 0){
        perror("bind error:");
        return false;
    }
    return true;
}

bool Socket::listen(int backlog){
    if(::listen(this->m_sockfd,backlog) < 0){
        perror("listen error:");
        return false;
    }
    return true;  
}

bool Socket::connect(const std::string &ip,int port){//connect is for client
    sockaddr_in caddr;
    memset(&caddr, 0, sizeof(caddr));
    caddr.sin_family=AF_INET;
    caddr.sin_addr.s_addr=inet_addr(ip.c_str());
    caddr.sin_port=htons(port);
    if(::connect(this->m_sockfd,(sockaddr*)&caddr,sizeof(caddr)) < 0){
        perror("connecton error:");
        return false;
    }
    return true;
}

bool Socket::close(){
    if(this->m_sockfd >= 0){
        ::close(this->m_sockfd);
        this->m_sockfd=0;
    }
    return true;
}

int Socket::accept(){
    sockaddr_in caddr;  //to memory client's informations
    socklen_t len=sizeof(caddr);
    int sockfd=::accept(this->m_sockfd,(sockaddr*)&caddr,&len);
    if(sockfd < 0){
        perror("accept error:");
        return -1;
    }
    return sockfd;
}

int Socket::recv(char * buf,int len){
    return ::recv(this->m_sockfd,buf,len,0);
}

int Socket::send(const char * buf,int len){
    return ::send(this->m_sockfd,buf,len,0);
}

bool Socket::set_non_blocking(){
    int flag=fcntl(this->m_sockfd,F_GETFL,0);
    if(flag < 0){
        perror("set_non_blocking F_GETFL error:");
        return false;
    }
    flag |= O_NONBLOCK;
    if(fcntl(this->m_sockfd,F_SETFL,flag) < 0){
        perror("set_non_blocking F_SETFL error:");
        return false;
    }
    return true;
}

bool Socket::set_send_buffer(int size){
    int buff_size=size;
    if( setsockopt(this->m_sockfd,SOL_SOCKET,SO_SNDBUF,&buff_size,sizeof(buff_size)) < 0){
        perror("set_send_buffer error:");
        return false;
    }
    return true;
}

bool Socket::set_recv_buffer(int size){
    int buff_size=size;
    if( setsockopt(this->m_sockfd,SOL_SOCKET,SO_RCVBUF,&buff_size,sizeof(buff_size)) < 0){
        return false;
    }
    return true;
}

bool Socket::set_linger(bool active, int seconds){ // set close wait time,blocking in closesocket when data not send all
    linger l;
    memset(&l, 0, sizeof(l));
    if (active) l.l_onoff = 1;
    else l.l_onoff = 0;
    l.l_linger = seconds;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0){
        perror("set_linger error:");
        return false;
    }
    return true;
}

bool Socket::set_keep_alive(){ // set keep_alive ,long connection
    int flag = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) < 0)
    {
        perror("set_keep_alive error:");
        return false;
    }
    return true;
}

bool Socket::set_reuse_addr(){ // set reuse port   ,set reuse in wait_time 
    int flag = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
    {
        perror("set_reuse_addr error:");
        return false;
    }
    return true;
}

bool Socket::set_reuse_port(){ // use for clients ,can use one port connect much server
    int flag = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0)
    {
        perror("set_reuse_port error:");
        return false;
    }
    return true;
}