#include "ServerSocket.h"


ServerSocket::ServerSocket():Socket(){
    
}

ServerSocket::ServerSocket(const std::string & ip,int port):Socket(ip,port){
    this->m_sockfd=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(this->m_sockfd < 0){
        perror("ServerSocket error");
        return ;
    }
    this->set_non_blocking();
    this->set_recv_buffer(10*1024);
    this->set_send_buffer(10*1024);
    this->set_linger(true,0);
    this->set_keep_alive();
    this->set_reuse_addr();
    this->bind(this->m_ip,this->m_port);
    this->listen(128);
}

ServerSocket:: ~ServerSocket(){
    close();
}
