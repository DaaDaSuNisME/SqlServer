#include "Server.h"

Server::Server():m_ip(""),m_min_num(0),m_max_num(0),m_connect_num(0),m_wait_time(0){

}

Server::~Server(){

}

bool Server::praseJsonFile(){
    std::ifstream ifs("Server/serverconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs,root);
    if(root.isObject()){
        this->m_ip=root["ip"].asString();
        this->m_port=root["port"].asInt();
        this->m_min_num=root["minNum"].asInt();
        this->m_max_num=root["maxNum"].asInt();
        this->m_connect_num=root["connectNum"].asInt();
        this->m_wait_time=root["waitTime"].asInt();
        return true;
    }
    perror("Server::praseJsonFile error:");
    return false;

}

void Server::start(){
    TaskDispacher *dispacher=Singleton<TaskDispacher>::instance();
    dispacher->init(this->m_min_num,this->m_max_num);//init Threadpool

    SocketHandler * handler=Singleton<SocketHandler>::instance();
    handler->listen(this->m_ip,this->m_port);//listen socket
    handler->handle(this->m_connect_num,this->m_wait_time);//create epoll and listen
}

void Server::set_listen(const std::string & ip,int port){
    this->m_ip=ip;
    this->m_port=port;
}

void Server::set_thread(int min_num,int max_num){
    this->m_min_num=min_num;
    this->m_max_num=max_num;
}

void Server::set_wait_time(int wait_time){
    this->m_wait_time=wait_time;
}