
#define L_IP4_ADDR      16
#define L_SESSION_ID    64

#define MAX_RCPT_TO     256
#define MAX_MAIL_FROM   256
#define MAX_DATA        1024*32
#define MAX_BUF_SIZE    1024*8

#define TRUE            1
#define FALSE           0

typedef enum smtpStatus {
    SMTP_STATUS_READY,
    SMTP_STATUS_READ_DATA
} smtpStatus_t;

typedef enum smtpMessageType {
    SMTP_UNKNOWN,
    SMTP_HELO,
    SMTP_MAIL,
    SMTP_RCPT,
    SMTP_QUIT,
    SMTP_DATA
} smtpMessageType_t;

#define SMTP_DISPATCH_OK 0
#define SMTP_DISPATCH_FAIL 1
#define SMTP_DISPATCH_END 2