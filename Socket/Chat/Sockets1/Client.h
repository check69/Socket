#include <string>

#if defined WIN32

    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <winsock.h>
    typedef int socklen_t;

#elif defined __linux__

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    typedef int SOCKET;
    #define INVALID_SOCKET  -1
    #define SOCKET_ERROR    -1
    #define closesocket (s) close(s);
    #define ioctlsocket (socket, typeProperty, iOption) ioctl(socket, typeProperty, iOption);

#endif

#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

class Client
{
    public:
                    Client          ();
                   ~Client          ();
        bool        init            (const char *hostname);
        void        end             ();

        bool        Send            (const char *message);
        char       *Recive          ();

        static int  getPort         () { return SERVER_PORT;}
        static int  getBuzzSize     () { return BUF_SIZE;}
        static int  getQsize        () { return QUEUE_SIZE;}

    protected:
        SOCKET      CreateSocket    ();
        bool        Connect         ();

    private:
        SOCKET           m_socket;
        struct hostent  *m_host;
        struct addrinfo *m_data;
};