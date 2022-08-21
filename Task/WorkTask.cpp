#include "WorkTask.h"
#include "../Socket/SocketHandler.h"


WorkTask::WorkTask(Socket *socket):Task(socket){

}

WorkTask:: ~WorkTask(){

}

void WorkTask::run(){
    SocketHandler *sockethandler=Singleton<SocketHandler>::instance();
    Socket * socket=static_cast<Socket *>(this->m_data);
    MsgHead head;
    int headsize=sizeof(head);
    int len=socket->recv((char *)&head,headsize);
    if (len == 0)//disconnected by client
    {
        perror("socket closed by peer");
        sockethandler->remove(socket);
        return;
    }
    if (len == -1 && errno == EAGAIN)//
    {
        perror("error msg: EAGAIN errno:");
        sockethandler->attach(socket);
        return;
    }
    if (len == -1 && errno == EWOULDBLOCK)//
    {
        perror("error msg: EWOULDBLOCK errno:");
        sockethandler->attach(socket);
        return;
    }
    if (len == -1 && errno == EINTR)
    {
        perror("error msg: EINTR errno:");
        sockethandler->attach(socket);
        return;
    }
    if (len != sizeof(head))//receive the error length of head
    {
        perror("error msg: length errno:");
        sockethandler->remove(socket);
        return;
    }
    int recv_buff_size=head.len,flag=head.cmd;
    char rdata[recv_buff_size]={0};
    len=socket->recv(rdata,recv_buff_size);

    if (len == -1 && errno == EAGAIN)// blocking mode ,and can't recv msg once
    {
        perror(" EAGAIN errno: ");
        sockethandler->remove(socket);
        return;
    }
    if (len == -1 && errno == EWOULDBLOCK)// blocking mode ,and can't recv msg once
    {
        perror("EWOULDBLOCK errno: ");
        sockethandler->remove(socket);
        return;
    }
    if (len == -1 && errno == EINTR)//connection interrupt
    {
        perror(" EINTR errno:");
        sockethandler->remove(socket);
        return;
    }
    if (len != (int)(head.len))//length error
    {
        perror("recv msg body error length: errno:");
        sockethandler->remove(socket);
        return;
    }
    std::cout<<"receive data:"<<rdata<<std::endl;

    MysqlPool *pool=Singleton<MysqlPool>::instance();
    std::shared_ptr<MysqlConn> conn=pool->getConnection();//?
    int ret=0;
    if(flag==UPDATE_FLAG){
        //std::cout<<"send data: first start"<<std::endl;
        conn->update(rdata);     
        head.len=0;
        ret=socket->send((char *)&head,headsize);
        //std::cout<<"send data: first end"<<std::endl;
    }else{
        //std::cout<<"send data: second start"<<std::endl;
        conn->update(rdata);
        string res=conn->getLine();
        int reslen=res.size();
        head.len=reslen;
        char sdata[reslen+headsize+1]={0};
        memcpy(sdata,&head,headsize);
        strcpy(sdata+headsize,res.c_str());
        ret=socket->send(sdata,sizeof(sdata));
        std::cout<<"send data: second end"<<std::endl;
    }
    if(ret <= 0){
        perror("EchoTask::run send error:");
        sockethandler->remove(socket);
        return ;
    }
    sockethandler->attach(socket); //finish task ,attach to Event_Poll again  
}

void WorkTask::destroy(){
    delete this;
}