#pragma once
#include <string>
#include "../Socket/SocketHandler.h"
#include "../Tools/Singleton.h"
#include "../Task/TaskDispacher.h"
#include <jsoncpp/json/json.h>
#include <fstream>

class Server{
public:
    Server();
    ~Server();
    void start();
    bool praseJsonFile();
    void set_listen(const std::string & ip,int port);
    void set_thread(int min_num,int max_num);
    void set_wait_time(int wait_time);
private:
    std::string m_ip;
    int m_port;
    int m_min_num;
    int m_max_num;
    int m_connect_num;
    int m_wait_time;
};