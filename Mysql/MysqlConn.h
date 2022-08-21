#pragma once
#include <iostream>
#include <mysql/mysql.h>
#include <chrono>
using namespace std;


class MysqlConn{
public:
    MysqlConn();
    ~MysqlConn();
    bool connect(string user,string passwd,string dbName,string ip,unsigned short port=3306);
    bool update(const string& sql);
    bool query(const string& sql);
    bool exec(const string& sql);//exec one command ,update or query
    bool next();
    string value(int index);
    string getLine();//get one query answer
    bool transaction();
    bool commit();
    bool rollback();
    void refleshAliveTime();
    long long getAliveTime();
private:
    void freeResult();
private:
    MYSQL * m_conn=nullptr;
    MYSQL_RES * m_result=nullptr;//query result
    MYSQL_ROW m_row=nullptr;//one row of query result
    std::chrono::steady_clock::time_point m_alivetime;
};

