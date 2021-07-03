#define BUFSIZE 128
#define FILEBUF 128

#define FileUpReq 1
#define FileAck 2
#define GetList 3
#define EndConnection 4

#define MAXPENDING 5

#ifdef _WIN32
    #include <winsock2.h> /* for socket(), connect(), send(), and recv() */
//    #pragma comment(lib, "ws2_32.lib") /* Winsock Library */
    #define LS "dir"
#elif __linux__
    #include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
    #include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
    #define LS "ls"
#endif