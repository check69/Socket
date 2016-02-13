#include "Server.h"
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>

using namespace std;

void   *SockAccept      (void *argument);
void    MsgManager      ();
void    sendBroadcast   (char *msg);
void    notifyClientOut (int sockfd);

list<SOCKET>    clients;
Server          myServer    = Server();
pthread_mutex_t Mutex       = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    pthread_t acceptSocket;

    if (!myServer.init())
    {
        exit(1);
    }

    pthread_create( &acceptSocket, NULL, SockAccept, NULL); // create a thread running function1

    while(true)
    {
        MsgManager();
    }

    myServer.end();

    return 0;
}


void *SockAccept(void * argument)
{
    while (true)
    {
        SOCKET sockfd = myServer.newSocketAccept();

        if (sockfd != INVALID_SOCKET)
        {
            pthread_mutex_lock(&Mutex);
            {
                clients.push_back(sockfd);
            }
            pthread_mutex_unlock(&Mutex);
        }
        Sleep(50);
    }

    return 0;
}

void MsgManager()
{
    pthread_mutex_lock(&Mutex);
    {
        for (list<SOCKET>::iterator it = clients.begin(); it != clients.end();)
        {
            char *msg = myServer.recvMessage(*it);

            if (msg != NULL)
            {
                if (strcmp(msg, "Quit") == 0)
                {
                    notifyClientOut(*it);

                    myServer.closeSocketAccept(*it);
                    it = clients.erase(it);
                }
                else
                {
                    sendBroadcast(msg);
                    ++it;
                }
            }
            else
            {
                notifyClientOut(*it);

                myServer.closeSocketAccept(*it);
                it = clients.erase(it);
            }
        }
    }
    pthread_mutex_unlock(&Mutex);
}

void sendBroadcast (char *msg)
{
    string message = msg;
    for (list<SOCKET>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2)
    {
        myServer.sendMessage(*it2, msg);
    }
}

void notifyClientOut (int sockfd)
{
    char quitMsg[BUF_SIZE];
    sprintf(quitMsg, "Se ha largado del chat %i", sockfd);
    sendBroadcast(quitMsg);
}