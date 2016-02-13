#include <iostream>
#include "Server.h"
#include <cstdio>
#include <stdlib.h>
#include <cstring>

Server::Server ()
{
    m_server = INVALID_SOCKET;
}

Server::~Server ()
{
    end();
}

bool Server::init ()
{
    std::cout<<"Initializating Server"<<std::endl;

    #ifdef WIN32
        m_version	= MAKEWORD(2,0);
        m_error		= WSAStartup(m_version, &m_wsaData);
        if(m_error != 0) return false;
        if(LOBYTE(m_wsaData.wVersion) != 2 || HIBYTE(m_wsaData.wVersion) != 0)
        {
            WSACleanup();
            return false;
        }
    #endif

        int socketfd = CreateSocket();

    if(socketfd == INVALID_SOCKET)
    {
        #ifdef WIN32
            WSACleanup();
        #endif

        return false;
    }

    m_server = socketfd;

    if (!bindServer() || !Listen())
    {
        return false;
    }

    u_long iMode = 1;

    int iResult = ioctlsocket(m_server, FIONBIO, &iMode);

    if (iResult != NO_ERROR)
    {
        printf("ioctlsocket failed with error: %ld\n", iResult);

        return false;
    }

    return true;
}

void Server::end ()
{
    if (m_server != INVALID_SOCKET)
    {
        closesocket(m_server);

        #ifdef WIN32
            WSACleanup();
        #endif
    }
}

int Server::CreateSocket()
{
    std::cout << "Create Server Socket" << std::endl;

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd == INVALID_SOCKET)
    {
        perror("ERROR en la apertura");
    }

    return sockfd;
}

bool Server::bindServer ()
{
    if (m_server == INVALID_SOCKET)
    {
        return false;
    }

    std::cout<<"Starting Server"<<std::endl;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_addr.s_addr  = htonl(INADDR_ANY);
    serverAddr.sin_port         = htons(SERVER_PORT);

    if (bind(m_server, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) == SOCKET_ERROR)
    {
        perror("ERROR in bind socket");
        closesocket(m_server);

        return false;
    }

    return true;
}

bool Server::Listen()
{
    if (m_server == INVALID_SOCKET)
    {
        return false;
    }

    if (listen(m_server, QUEUE_SIZE) == SOCKET_ERROR)
    {
        perror ("ERROR in listen");
        closesocket(m_server);

        return false;
    }

    return true;
}

SOCKET Server::newSocketAccept ()
{
    if (m_server == INVALID_SOCKET)
    {
        return m_server;
    }

    struct sockaddr_in clientAddr;
    int length = sizeof(clientAddr);

    SOCKET sockAccept = accept(m_server, (struct sockaddr *)&clientAddr, (socklen_t *)(&length));
    if (sockAccept == INVALID_SOCKET)
    {
        return sockAccept;
    }

    return sockAccept;
}

void Server::closeSocketAccept (SOCKET sockfd)
{
    if (sockfd != INVALID_SOCKET)
        closesocket(sockfd);
}

char *Server::recvMessage (SOCKET sockfd)
{
    if (sockfd == INVALID_SOCKET)
        return NULL;

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    if (recv(sockfd, buf, BUF_SIZE, 0) == SOCKET_ERROR)
    {
        #ifdef WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                return "";
            }
        #elif defined __linux__
            if (errno == EWOULDBLOCK)
            {
                return "";
            }
        #endif

        return NULL;
    }
    else if (buf == NULL)
    {
        return "";
    }
    else
    {
        return buf;
    }
}

void Server::sendMessage (SOCKET sockfd, char *msg)
{
    if (sockfd != INVALID_SOCKET && msg != NULL)
    {
        std::string result = msg;
        send (sockfd, result.c_str(), result.size(), 0);
    }
}

void Server::run ()
{
    if (m_server == INVALID_SOCKET)
    {
        return;
    }

    struct sockaddr_in clientAddr;
    int length = sizeof(clientAddr);

    SOCKET sockAccept = accept(m_server, (struct sockaddr *)&clientAddr, (socklen_t *)(&length));

    if (sockAccept == INVALID_SOCKET)
    {
        perror("Accept failed");

        #ifdef WIN32
            printf("%i", WSAGetLastError());
        #endif
    }
    else
    {
        std::string result = nullToEmpty(recvMessage(sockAccept));
        std::string ip = inet_ntoa(clientAddr.sin_addr);
        printf("Resultado: %s\tClient: %s\tPort: %i\n", result.c_str(), ip.c_str(), clientAddr.sin_port);

        closesocket(sockAccept);
    }
}

void Server::ManageInfo(char* message)
{
    std::cout<<"Message recived : "<<message<<std::endl;
}

const char* nullToEmpty( char const* s)
{
    return (s ? s : "");
}