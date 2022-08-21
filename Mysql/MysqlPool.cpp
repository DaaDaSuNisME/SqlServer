#include "MysqlPool.h"
#include <fstream>

MysqlPool::MysqlPool(){
    if(!this->parseJsonFile()){
        perror("parseJsonFile");
        return ;
    }
    for(int i=0;i<this->m_minSize;++i){
        this->addConnection();
    }
    std::thread producer(&MysqlPool::produceConnection,this);
    std::thread consumer(&MysqlPool::deleteConnection,this);
    producer.detach();
    consumer.detach();
}

MysqlPool::~MysqlPool(){
    while(!this->m_connectQueue.empty()){
        MysqlConn * conn=this->m_connectQueue.front();
        this->m_connectQueue.pop();
        delete conn;
    }
}

bool MysqlPool::parseJsonFile(){
    ifstream ifs("Mysql/dbconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs,root);
    if(root.isObject()){
        this->m_ip=root["ip"].asString();
        this->m_port=root["port"].asInt();
        this->m_user=root["userName"].asString();
        this->m_passwd=root["password"].asString();
        this->m_dbName=root["dbName"].asString();
        this->m_minSize=root["minSize"].asInt();
        this->m_maxSize=root["maxSize"].asInt();
        this->m_maxIdletime=root["maxIdleTime"].asInt();
        this->m_timeout=root["timeout"].asInt();
        return true;
    }
    return false;

}

std::shared_ptr<MysqlConn>  MysqlPool::getConnection(){
    std::unique_lock<std::mutex> lock(this->m_mtx);
    while(this->m_connectQueue.empty()){
        
        /*
        if(std::cv_status::timeout==this->m_consumer.wait_for(lock,std::chrono::milliseconds(m_timeout))){
            if(this->m_connectQueue.empty()){
                continue;
            }
        }
        */
        
        this->m_consumer.wait(lock);
    }
    std::shared_ptr<MysqlConn> connptr(this->m_connectQueue.front(),[this](MysqlConn* conn){
        conn->refleshAliveTime();
        std::lock_guard<std::mutex> lock(this->m_mtx);
        this->m_connectQueue.push(conn);
    });
    this->m_connectQueue.pop();
    this->m_producer.notify_all();
    return connptr;
}

void MysqlPool::produceConnection(){
    while(true){
        std::unique_lock<std::mutex> lock(this->m_mtx);
        while(m_connectQueue.size() >= this->m_minSize){
            this->m_producer.wait(lock);
        }
        this->addConnection();
        this->m_consumer.notify_all();
    }
}

void MysqlPool::deleteConnection(){
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::lock_guard<std::mutex> lock(this->m_mtx);
        while(m_connectQueue.size() > this->m_minSize){
            MysqlConn * conn=this->m_connectQueue.front();
            if(conn->getAliveTime()  >= this->m_maxIdletime){
                this->m_connectQueue.pop();
                delete conn;
            }else{
                break;
            }
        }
    }
}

void MysqlPool::addConnection(){
    MysqlConn * conn=new MysqlConn;
    conn->connect(this->m_user,this->m_passwd,this->m_dbName,this->m_ip,this->m_port);
    conn->refleshAliveTime();
    this->m_connectQueue.push(conn);
}



