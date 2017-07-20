//
// Created by Alex C. on 19.07.17.
//

#include "tcpserver.h"
/*  ____   _   _  ____   _      ___  ____  ____
   |  _ \ | | | || __ ) | |    |_ _|/ ___|/ ___|
   | |_) || | | ||  _ \ | |     | || |    \___ \
   |  __/ | |_| || |_) || |___  | || |___  ___) |
   |_|     \___/ |____/ |_____||___|\____||____/
 */

/**
 * Constructor function for TcpServer.
 * @param port The port number used by server to listen for incoming connections. Value must me in interval [1024-65535]
 * @throw Error in case a socket handler can't be created.
*/
TcpServer::TcpServer(uint16_t port)
{
    if (port < 1024 || port > 65535) /// If port does not match the interval throw an error
    {
        error("The listening port must be in interval [1024-65535]");
    }
    this->port_number = port;
    
    /// Attempting to create a socket handler for server
    hServer = (socklen_t) socket(AF_INET, SOCK_STREAM, 0);
    if (hServer == -1)
    {
        error("Can't open socket!");
    }
    
    /// Also set Keep-Alive flag by default
    int optval = 1;
    int optlen = sizeof(optval);
    if (setsockopt(hServer, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
    {
        close(hServer);
        error("Can't set keep-alive flag!");
    }
}

/**
 * Destructor for TcpServer class
 */
TcpServer::~TcpServer()
{
    close(hServer);
    close(hClient);
}

/**
 * Get server state.
 * @return Returns true in case server was created and binded and false otherwise;
 */
bool TcpServer::getServerStatus()
{
    return this->serverListening;
}

/**
 * Start TCP Server and wait for incoming connection
 * @return This function return TRUE in case server succesfullt created and binded and FALSE otherwise
 */
bool TcpServer::startServer()
{
    struct sockaddr_in server;  // Structure used to store server info
    
    // Setting up server details
    memset(&server, 0, sizeof(server));     // cleaning up struct
    server.sin_family = AF_INET;            // address family. leave AF_INET
    server.sin_addr.s_addr = INADDR_ANY;    // accept any incoming messages
    server.sin_port = htons(port_number);   // host-to-network short
    
    // Binding socket we just created to an address!
    if (bind(hServer, (const sockaddr *) &server, sizeof(server)) < 0)
    {
        setLastError("Can't bind server socket! Are you sure if this port is not already used?");
        return false;
    }
    
    // Start socket listening for connections
    if (listen(hServer, 3) <
        0)    // 3 is the number of connections that can be waiting while the process is handling a particular connection
    {
        setLastError("Can't start listening on given port!");
        return false;
    }
    
    this->serverListening = true;
    return true;
}

/**
 * Stop TCP Server if started before
 * @return Returns TRUE in case of success and FALSE otherwise
 */
bool TcpServer::stopServer()
{
    if (this->getServerStatus() == false)
    {
        setLastError("Server was not even started to be stopped!");
        return false;
    }
    
    close(hClient); // Close connection with client if any
    close(hServer); // Shutdown server
    
    return true;
}

/**
 * Wait and handle incoming clients. This function will block thread. Call it from another thread in case
 * you want to handle multiple clients
 * When a client connects, is will have assigned the handler @var hClient. Use that structure to handle client.
 * @return Returns TRUE when first client was connected and stop blocking thread
 */
void TcpServer::waitForClients()
{
    struct sockaddr_in sockaddr_client;                     //Structure to store client details
    socklen_t client_addr_len = sizeof(sockaddr_client);    // size of structure
    
    while (1)    // Wait until a client connects
    {
        //Accepting client connection. It actually block process till the client is getting connected
        hClient = (socklen_t) accept(hServer, (struct sockaddr *) &sockaddr_client, &client_addr_len);
        if (hClient >= 0)
        {
            //Let's also set Keep-Alive Flag
            int optval = 1;
            int optlen = sizeof(optval);
            setsockopt(hClient, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
            
            // Extra check. If client is writable :)
            if (write(hClient, "Welcome  to IMU Server!\n", 24) >= 23)
            {
                return;
            }
        }
    }
}

/**
 * This is a loop and is running on another thread.
 * When client is sending data, this function callback function for incoming data
 * That function is virtual and needs to be implemented in order to handle received data
 */
void TcpServer::clientWaitForIncomingData()
{
    if(clientIncomingDataCallback == NULL)
    {
        this->setLastError("No callback function for data incoming set!");
        return;
    }
    
    // We read message chunk by chunk so we need a buffer to store data
    char msgChunk[BUFFSIZE];
    
    // And a string to store result asuming that we won't receive huge amount of data
    std::string recvData = "";
    
    while (true)   // Keep listening
    {
        memset(&msgChunk, 0, sizeof(msgChunk));
        int readResult = (int) read(hClient, msgChunk, sizeof(msgChunk));
        
        if( readResult == 0) //reached end of file (EOF)
        {
            recvData += msgChunk;
            recvData.erase( std::remove(recvData.begin(), recvData.end(), '\n'), recvData.end() );  //remove newlines first
            recvData.push_back('\0');
            
            //clientIncomingDataCallback(recvData);   //send data to user to handle
            recvData = "";  //We need to reset counter so we can read again
        }
        else if(readResult > 0)
        {
            recvData += msgChunk;
            recvData.erase( std::remove(recvData.begin(), recvData.end(), '\n'), recvData.end() );  //remove newlines first
    
            clientIncomingDataCallback(recvData);   //send data to user to handle
            recvData = "";  //We need to reset counter so we can read again
        }
        //Don't need to handle readResult < 0 - we catch here only valid received data
    }
}

int TcpServer::clientWrite(const char *data)
{
    size_t dataLen = strlen(data);
    ssize_t sendBytes = write(hClient, data, dataLen);
    
    // Check how many bytes were written
    if(sendBytes < 0)
    {
        this->setLastError("Error on writing data to client socket!");
        return -1;
    }
    
    return (int)sendBytes;
}

/*  ____   ____   ___ __     __ _   _____  _____  ____
   |  _ \ |  _ \ |_ _|\ \   / // \ |_   _|| ____|/ ___|
   | |_) || |_) | | |  \ \ / // _ \  | |  |  _|  \___ \
   |  __/ |  _ <  | |   \ V // ___ \ | |  | |___  ___) |
   |_|    |_| \_\|___|   \_//_/   \_\|_|  |_____||____/
 */

/**
 * Throw an exception if machine doesn't meet requirements
 * @param message The error message!
 * @throw The cause!
 */
void TcpServer::error(std::string message)
{
    std::string errMsg = "TCP Server Error: ";  /// Error throwing header
    errMsg += message;
    throw (errMsg);
}

/**
 * Set last error
 * @param err The error string description
 */
void TcpServer::setLastError(std::string err)
{
    this->lastError = err;
}

/**
 * Function to grab last error if any
 * @return The description of last error.
 */
std::string TcpServer::getLastError()
{
    return this->lastError;
}
