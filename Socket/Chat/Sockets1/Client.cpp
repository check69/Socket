#include <iostream>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include "Client.h"

Client::Client ()
{
    m_socket = INVALID_SOCKET;
}

Client::~Client ()
{
    end();
}

bool Client::init (const char *hostname)
{
    std::cout << "Initializating Client" << std::endl;

    #ifdef WIN32

        WSADATA wsaData;
        int error = WSAStartup(MAKEWORD(2, 0), &wsaData);
        if(error != 0) return false;
        if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
        {
            WSACleanup();
            return false;
        }

    #endif

    if(!hostname)
    {
        hostname = "localhost";
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(hostname, "12345", &hints, &m_data);

    m_socket = CreateSocket();

    if(m_socket == INVALID_SOCKET)
    {
        #ifdef WIN32
            WSACleanup();
        #endif

        return false;
    }

    if(!Connect())
    {
        return false;
    }

    u_long iMode = 0;

    int iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
    {
        printf("ioctlsocket failed with error: %ld\n", iResult);
        return false;
    }

    return true;
}

void Client::end ()
{
    if(m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);

        #ifdef WIN32
            WSACleanup();
        #endif
    }
}

SOCKET Client::CreateSocket ()
{
    std::cout << "Create Socket" << std::endl;

    int sockfd = socket(m_data->ai_family, m_data->ai_socktype, m_data->ai_protocol);

    if (sockfd == INVALID_SOCKET)
    {
        perror("ERROR en la apertura");
    }

    return sockfd;
}

bool Client::Connect ()
{
    if (m_socket == INVALID_SOCKET)
        return false;

    if (connect(m_socket, (struct sockaddr *)m_data->ai_addr, m_data->ai_addrlen) == SOCKET_ERROR)
    {
        perror("connection error");
        closesocket(m_socket);
        return false;
    }

    return true;
}

bool Client::Send (const char *message)
{
    if (m_socket == INVALID_SOCKET)
    {
        return false;
    }

    if (send(m_socket, message, strlen(message)+1, 0) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

char *Client::Recive ()
{
    if (m_socket == INVALID_SOCKET)
    {
        return NULL;
    }

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    if (recv(m_socket, buf, BUF_SIZE, 0) == SOCKET_ERROR)
    {
        return NULL;
    }
    else
    {
        return buf;
    }

    return NULL;
}