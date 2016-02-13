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
    #include <unistd.h>
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(s) close(s);
    #define ioctlsocket(socket, typeProperty, iOption) ioctl(socket, typeProperty, iOption);

#endif

#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

const char *nullToEmpty (char const *s);

class Server
{
    public:
        Server              ();
        ~Server              ();
        bool        init                ();
        void        end                 ();

        SOCKET      newSocketAccept     ();
        void        closeSocketAccept   (SOCKET sockfd);
        char       *recvMessage         (SOCKET sockfd);
        void        sendMessage         (SOCKET sockfd,char *message);
        void        run                 ();
        void        ManageInfo          (char *message);

        static int  getPort             () {
            return SERVER_PORT;
        }
        static int  getBuzzSize         () {
            return BUF_SIZE;
        }
        static int  getQsize            () {
            return QUEUE_SIZE;
        }

    protected:
        int         CreateSocket        ();
        bool        bindServer          ();
        bool        Listen              ();

    private:
        #ifdef WIN32
        WSADATA      m_wsaData;
        WORD         m_version;
        #endif

        SOCKET           m_server;
        int	             m_error;
        char             m_buf[BUF_SIZE];
};