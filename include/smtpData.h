
#define L_IP4_ADDR      16
#define L_SESSION_ID    64

#define MAX_RCPT_TO     256
#define MAX_MAIL_FROM   256
#define MAX_DATA        1024*32
#define MAX_BUF_SIZE    1024*8

#define TRUE            1
#define FALSE           0

typedef enum smtp_status {
    SMTP_STATUS_READY,
    SMTP_STATUS_READ_DATA
} smtp_status_t;

typedef enum smtp_message_type {
    SMTP_UNKNOWN,
    SMTP_HELO,
    SMTP_MAIL,
    SMTP_RCPT,
    SMTP_QUIT,
    SMTP_DATA
} smtp_message_type_t;

#define SMTP_DISPATCH_OK 0
#define SMTP_DISPATCH_FAIL 1
#define SMTP_DISPATCH_END 2