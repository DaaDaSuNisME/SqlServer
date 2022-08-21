#pragma once

#include "Socket.h"

class ServerSocket : public Socket{
public:
    ServerSocket();
    ServerSocket(const std::string & ip,int port);
    virtual ~ServerSocket();
};