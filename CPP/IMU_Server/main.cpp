#include <iostream>

#include "tcpserver.h"

int main()
{
    // TCP Server Handler
    TcpServer server(1337);
    
    std::cout << "Starting TCP Server....";
    if(!server.startServer())
    {
        std::cout << "\x1b[1;31m" << "FAILED\n" << "\n" << "\x1b[0m";
        std::cout << "\x1b[31m" << server.getLastError() << "\n" << "\x1b[0m";
        return 1;
    }
    std::cout << "\x1b[1;32m" << "OK\n" << "\x1b[0m";
    
    std::cout << "Waiting for clients...." << std::flush;
    server.waitForClients();    //Don't go forward until at least one client connects
    std::cout << "\x1b[1;32m" << "OK\n" << "\x1b[0m";
}