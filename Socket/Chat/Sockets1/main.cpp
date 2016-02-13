#include "Client.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

using namespace std;

bool        SendMessage     ();
void       *ReceiveMessage  (void *argument);
const char *nullToEmpty     (char const* s);

Client myClient = Client();

int main (int argc, char **argv)
{
    pthread_t revcMsg;

    if (argc > 1)
    {
        if (!myClient.init(argv[1]))
        {
            exit(1);
        }
    }
    else
    {
        if (!myClient.init(NULL))
        {
            exit(1);
        }
    }

    pthread_create(&revcMsg, NULL, ReceiveMessage, NULL); // create a thread running receiveMessage
    
    while(SendMessage());

    myClient.end();

    return 0;
}

void *ReceiveMessage (void *argument)
{
    while (true)
    {
        char *msg = myClient.Recive();
        if (msg == NULL)
        {
            perror("Connection server lost");
            return 0;
        }

        string result = msg;
        printf ("%s\n", result.c_str());
    }

    return 0;
}

bool SendMessage ()
{
    bool cond = true;

    char data[BUF_SIZE];
    cin.getline(data, BUF_SIZE);

    if (strcmp(data, "Quit") == 0)
    {
        cond = false;
    }

    if (!myClient.Send(data))
    {
        return false;
    }

    return cond;
}

const char* nullToEmpty (char const* s)
{
    return (s ? s : "");
}