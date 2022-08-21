#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <jsoncpp/json/json.h>
#include "MysqlConn.h"

using namespace std;


class MysqlPool{
public:
    MysqlPool();
    ~MysqlPool();
    std::shared_ptr<MysqlConn> getConnection();
private:
    bool parseJsonFile();       //read json file
    void produceConnection();   //producer
    void deleteConnection();    //delete connection
    void addConnection();       //
private:
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;
    int m_minSize;
    int m_maxSize;
    int m_timeout;
    int m_maxIdletime;
    mutex m_mtx;
    condition_variable m_producer;
    condition_variable m_consumer;
    queue<MysqlConn *> m_connectQueue;//shared resource
    
};

