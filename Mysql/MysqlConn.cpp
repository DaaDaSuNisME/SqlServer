#include "MysqlConn.h"

MysqlConn::MysqlConn(){
    this->m_conn=mysql_init(nullptr);
    mysql_set_character_set(this->m_conn,"utf8");
}

MysqlConn::~MysqlConn(){
    if(this->m_conn){
        mysql_close(this->m_conn);
    }
    freeResult();
}

bool MysqlConn::connect(string user,string passwd,string dbName,string ip,unsigned short port){
    MYSQL * rptr=mysql_real_connect(this->m_conn,ip.c_str(),user.c_str(),passwd.c_str(),dbName.c_str(),port,nullptr,0);
    if(!rptr) return false;
    return true;
}

bool MysqlConn::update(const string& sql){
    if(mysql_query(this->m_conn,sql.c_str())){
        perror("update");
        return false;
    }
    return true;
}

bool MysqlConn::query(const string& sql){
    freeResult();//clear Result
    if(mysql_query(this->m_conn,sql.c_str())){
        perror("query");
        return false;
    }
    this->m_result=mysql_store_result(this->m_conn);
    return true;
}

bool MysqlConn::exec(const string& sql){
    freeResult();//clear Result
    if(mysql_query(this->m_conn,sql.c_str())){
        perror("query");
        return false;
    }
    this->m_result=mysql_store_result(this->m_conn);
    if(this->m_result) return  true;
    return false;
}

bool MysqlConn::next(){
    if(this->m_result){
        this->m_row = mysql_fetch_row(this->m_result);
        if(this->m_row) return true;
    }
    return false;
}

string MysqlConn::value(int index){
    int rowCount=mysql_num_fields(this->m_result);
    if(index>=rowCount || index<0){
        return string();
    }
    char * val = m_row[index];
    int length = mysql_fetch_lengths(this->m_result)[index];
    return string( val,length );
}

string MysqlConn::getLine(){
    string res;
    for(int i=0;;i++){
        string node=this->value(i);
        if(node=="") break;
        res+=" "+node;
    }
    this->next();
    return res.substr(1);
}

bool MysqlConn::transaction(){
    return mysql_autocommit(this->m_conn,false);
}

bool MysqlConn::commit(){
    return mysql_commit(this->m_conn);
}

bool MysqlConn::rollback(){
    return mysql_rollback(this->m_conn);
}

void MysqlConn::freeResult(){
    if(this->m_result){
        mysql_free_result(this->m_result);
        this->m_result=nullptr;
    }
}

void MysqlConn::refleshAliveTime(){
    this->m_alivetime=std::chrono::steady_clock::now();
}

long long MysqlConn::getAliveTime(){
    std::chrono::nanoseconds res=chrono::steady_clock::now()-this->m_alivetime;
    chrono::milliseconds millires=chrono::duration_cast<chrono::milliseconds> (res);
    return millires.count();
}