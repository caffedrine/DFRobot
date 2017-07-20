#include <iostream>

#include "tcpserver.h"

// TCP Server Handler
TcpServer server(1338);

/**
 * This function is used to handle input stream from connected clients
 */
void recvDataCallback(std::string data)
{
    std::cout << "\x1b[1;32m" << "CLIENT: " << "\x1b[0m" << data << std::endl;
    
    // Write back an acknowledge to client
    const char ack[] = "ACK, data received!\n";
    if(server.clientWrite(ack) > 0)
        std::cout << "\x1b[1;31m" << "SERVER: " << "\x1b[0m" << ack;
}

int main()
{
    /// Important: Set function which will handle client response
    server.clientIncomingDataCallback = &recvDataCallback;
    
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
    
    std::cout << "Enable data listening....\n" << std::flush;
    server.clientWaitForIncomingData(); //Or you can run this on another thread and then do your stuff
    //std::cout << "\x1b[1;32m" << "OK\n" << "\x1b[0m";
    
    // Don't forget to stop server
    server.stopServer();
}

