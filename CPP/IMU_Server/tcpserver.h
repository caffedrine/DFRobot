//
// Created by Alex C. on 19.07.17.
//
#ifndef IMU_SERVER_TCPSERVER_H
#define IMU_SERVER_TCPSERVER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>
#include <iostream>

/**
 * Simple sockets wrapper.
 * The class which will handle all data exchanged between server and client(s).
 * The server will listen by default on all internal and external IP Addresses assigned to machine.
 * Use "hostname -I" to see internal IP you can use to connect. Also you can connect using "localhost" or
 * 127.0.0.1.
 */
class TcpServer
{
public:
    TcpServer(uint16_t port);   // Constructor - Listening port must be passed
    virtual ~TcpServer();
    
    bool startServer();         // Function to start the server
    bool stopServer();          // Function to stop server
    bool getServerStatus();     // Get status of server: started/stopped
    void waitForClients();      // Wait for incoming clients to connect to created server
    
    bool getClientStatus();     // Get status of client(s): connected/not connected
    
    int clientWrite(const char *) const;  // Write data to first connected client
    virtual std::string clientReadString();       // Read received string from first client

    //Additional methods
    std::string getLastError(); //Return last error if any
private:
    /// Server and Client sockets handlers
    socklen_t hServer, hClient;

    /// Port used by server to listen
    uint16_t port_number;
    
    /// A variable to store the state of server.
    bool serverListening = false; // This value became true on server is created and listen(...) was called
    
    // Some internal methods
    void setLastError(std::string err); // Set error when occurred
    
    // Wait for incoming data form clients but from another thread
    void waitForIncomingClientData();
    
    // Throw critical errors
    void error(std::string);
};

#endif //IMU_SERVER_TCPSERVER_H
