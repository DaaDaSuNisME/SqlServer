#include "Server/Server.h"
#include "Tools/Singleton.h"


int main(){
    Server *server=Singleton<Server>::instance();
    server->praseJsonFile();
    server->start();
    return 0;
}