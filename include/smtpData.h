
#define L_IP4_ADDR      16

#define MAX_BUF_SIZE    1024*8

typedef struct {
    int     SockFd  ;
    int     PortNum ;
    char    StrIP4   [ L_IP4_ADDR ];
} SmtpSvrArgs_t ;
