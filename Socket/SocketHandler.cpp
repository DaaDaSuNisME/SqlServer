#include "SocketHandler.h"

SocketHandler::SocketHandler(){
    
}

SocketHandler::~SocketHandler(){
    if(this->m_epoller){
        delete this->m_epoller;
        this->m_epoller=nullptr;
    }
    if(this->m_listensocket){
        delete this->m_listensocket;
        this->m_listensocket=nullptr;
    }
}

void SocketHandler::listen(const std::string & ip,int port){
    this->m_listensocket=new ServerSocket(ip,port);
} 

void SocketHandler::attach(Socket *socket){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    this->m_epoller->add(socket->m_sockfd,socket,(EPOLLONESHOT | EPOLLIN | EPOLLHUP | EPOLLERR));
}

void SocketHandler::detach(Socket *socket){
    std::lock_guard<std::mutex> locker(this->m_mtx);
    this->m_epoller->del(socket->m_sockfd,socket,(EPOLLONESHOT | EPOLLIN | EPOLLHUP | EPOLLERR));
}

void SocketHandler::remove(Socket *socket){
    socket->close();
    this->m_socketpool.release(socket);
}

void SocketHandler::handle(int maxconnection,int timewait){
    this->m_epoller=new Epoller(false);
    this->m_epoller->create(maxconnection);
    this->m_epoller->add(this->m_listensocket->m_sockfd,this->m_listensocket,(EPOLLIN | EPOLLHUP | EPOLLERR));
    this->m_socketpool.init(maxconnection);
    while(true){
        int num=this->m_epoller->wait(timewait);
        if(num == 0){
            continue;
        }
        for(int i=0;i<num;++i){
            if(this->m_listensocket==this->m_epoller->m_evs[i].data.ptr){
                //listen event
                Socket *socket=this->m_socketpool.allocate();
                if( !socket ){
                    perror("SocketHandler handle listen error:");
                    continue;
                }
                int sockfd=this->m_listensocket->accept();
                socket->m_sockfd=sockfd;
                socket->set_non_blocking();
                this->attach(socket);
            }
            else {
                //connection event
                Socket *socket=static_cast<Socket *>(this->m_epoller->m_evs[i].data.ptr);
                if( this->m_epoller->m_evs[i].events & EPOLLHUP){
                    perror("SocketHandler handle client disconnect:");
                    detach(socket);
                    remove(socket);
                }
                else if( this->m_epoller->m_evs[i].events & EPOLLERR ){
                    perror("SocketHandler handle connection second error:");
                    detach(socket);
                    remove(socket);
                }
                else if( this->m_epoller->m_evs[i].events & EPOLLIN ){
                    detach(socket);// detach socket from listen_Events , after finish work will attach again
                    Task *task=TaskFactory::produce(socket);//use socket to create a communication Task
                    Singleton<TaskDispacher>::instance()->assign(task);//assign Task or add Task to TaskPool
                }


            }
        }
    }
}
