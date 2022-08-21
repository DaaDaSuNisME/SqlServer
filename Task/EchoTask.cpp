#include "EchoTask.h"
#include "../Socket/SocketHandler.h"//SocketHangle included TaskDispacher,so TaskDispacher can't include SocketHandler circle

EchoTask::EchoTask(Socket *socket):Task(socket){

}

EchoTask:: ~EchoTask(){

}

void EchoTask::run(){
    SocketHandler *sockethandler=Singleton<SocketHandler>::instance();
    Socket * socket=static_cast<Socket *>(this->m_data);
    int length;
    socket->recv((char *)&length,sizeof(length));
    length=ntohl(length);

    char rdata[length]={0};
    int len=socket->recv(rdata,sizeof(rdata));
    if(len==0){
        perror("EchoTask::run first error:");
        sockethandler->remove(socket);//romove : close socketfd and push Socket* into Epoller and ObjectPool;  
        return ;
    }
    else if(len<0){
        perror("EchoTask::run second error:");
        sockethandler->remove(socket);
        return ;
    }else if(len!=length){
        perror("EchoTask::run third error:");
        sockethandler->remove(socket);
        return ;
    }
    std::cout<<"receive data:"<<rdata<<std::endl;

    MysqlPool *pool=Singleton<MysqlPool>::instance();
    std::shared_ptr<MysqlConn> conn=pool->getConnection();//?
    char sql[1024]={0};
    sprintf(sql,"insert into students values(001,003,'祭蓝航',200.5,'女',21,'正常')");
    conn->update(sql);
    /*
    
    //string sql=rdata;
    char sql[1024]={0};
    sprintf(sql,"insert into students values(001,003,'祭蓝航',200.5,'女',21,'正常')");
    if(!conn->exec(sql)) return ;

    string res=conn->getLine();
    int reslen=htonl(res.size());
    char sdata[reslen+4]={0};
    memcpy(sdata,&reslen,4);
    memcpy(sdata+4,rdata,reslen);
    int ret=socket->send(sdata,sizeof(sdata));
*/
    
    char sdata[len+4]={0};
    int netlen=htonl(len);
    memcpy(sdata,&netlen,4);
    memcpy(sdata+4,rdata,len);
    int ret=socket->send(sdata,len+4);
    if(ret <= 0){
        perror("EchoTask::run send error:");
        sockethandler->remove(socket);
        return ;
    }
    sockethandler->attach(socket); //finish task ,attach to Event_Poll again
}

void EchoTask::destroy(){
    delete this;
}