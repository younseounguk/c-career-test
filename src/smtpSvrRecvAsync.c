#include "main.h"

/* TODO 과제 2 공통
 *  비동기 통신방식으로 데이터가 수신 처리되어야 합니다.
 *  기존 소스코드 변경 및 파일이 추가하는데에는 전혀 제약사향이 없습니다.
 *  socket 정보 관리는 session을 통해서 관리되어야 합니다.
 *  smtpHandleInboundConnection로직을 통해 Connection을 맺는걸 권유 드립니다.(해당 로직을 사용안하셔도 무방합니다.)
 *
 */

#define MAX_LIST 512

char* pop_id();
int store_id(char* session_id);

pthread_mutex_t getter_lock  = PTHREAD_MUTEX_INITIALIZER;
int pop_index = 0;
int store_index = 0;
char* session_id_list[MAX_LIST] = {0, };

char* pop_id() {
    // session id list에 저장된 id pop
    char* return_id = NULL;

    pthread_mutex_lock(&getter_lock);  
    if(session_id_list[pop_index] != NULL) {     
        
        return_id = session_id_list[pop_index];
        session_id_list[pop_index] = NULL;
        pop_index++;
        
        if (pop_index == MAX_LIST) {
            pop_index = 0;
        }    
        
    }
    pthread_mutex_unlock(&getter_lock);
    return return_id;
}

int store_id(char* session_id) {
    // session id 저장
    if(session_id_list[store_index] != NULL) {
        msleep(100);
        return 1;
    }

    session_id_list[store_index] = session_id;
    store_index++;

    if(store_index == MAX_LIST) {
        store_index = 0;
    }

    return 0;
}

void smtpWaitAsync(int server_fd) {
        /* TODO 과제 2-1
        *  smtpSvrRecvAsync.c 파일은 비동기 처리를 이용하여 데이터를 제어 하는 로직이 작성 되어 있습니다.
        *  N개의 H_SERVER_WORK_TH 스레드와 하나의 smtpWaitAsync 스레드가 동시에 동작하고 있습니다.
        *  smtpWaitAsync Function에서 Connection을 맺고(smtpHandleInboundConnection로직 사용 권유)
        *  비동기 통신이 가능하게 개발한후 session정보를 WorkThread로 전달하는 로직을 개발하여야 합니다.
        */

    struct timeval timeout;
    fd_set reads, temps;
    int fd;
    int fd_max;

    // fd 초기화및server_fd에 설정
    FD_ZERO(&reads);
    FD_SET(server_fd, &reads);
    fd_max = server_fd;

    while(1) {
        temps = reads;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        if(select(fd_max + 1, &temps, 0, 0, &timeout) < 0) {
            // err log 출력
            LOG(LOG_CRT, "Err. select error.");
            return;
        }
        
        // 설정된 fd에서 클라이언트 연결 요청 확인
        for(fd = 0; fd < fd_max+1; fd++) {
            if(FD_ISSET(fd, &temps)) {
                if(fd == server_fd) {
                    // client 연결 수락 및 세션 생성
                    smtp_session_t* cur_session = smtpHandleInboundConnection(server_fd);
                    
                    // 세션 정보 저장
                    addSmtpSession(cur_session);

                    while(store_id(cur_session->session_id) != 0);
                }
            }
        }
    }
    return;
}

void *H_SERVER_WORK_TH(void *args) {
    int nLine, nErr;
    char buf[MAX_BUF_SIZE];
    smtp_session_t *session = NULL;

    char* session_id = NULL;    

    while (!g_sys_close) {

        /* TODO 과제 2-2
        *  session정보를 해당 위치에 받아 올수있게 개발하여야 합니다.
        */
        
        session = getSmtpSession(session_id);
        
        if (session == NULL) {
            msleep(25);
            session_id = pop_id();
            continue;
        }
    
        if ((nLine = smtpReadLine(session->sock_fd, buf, sizeof(buf))) <= 0) {
            LOG (LOG_INF, "%s : %sSMTP Connection closed%s : fd = %d, session_id=%s\n", __func__, C_YLLW, C_NRML,
                 session->sock_fd,
                 session->session_id);
            delSmtpSession(session->session_id);
            continue;
        }

        if ((nErr = doSmtpDispatch(session, buf)) != SMTP_DISPATCH_OK) {
            if (nErr == SMTP_DISPATCH_FAIL) {
                LOG(LOG_INF, "Smtp connection close by error!");
            }

            delSmtpSession(session->session_id);
            continue;
        }

    }
    return NULL;
}


int smtpStartWorkThreads(int n_work_threads) {
    int nErr;
    pthread_t clientTh;
    pthread_attr_t clientThAttr;
    pthread_attr_init(&clientThAttr);

    nErr = pthread_attr_setstacksize(&clientThAttr, (10 * 1024 * 1024));

    for (int i = 0; i < n_work_threads; i++) {
        if ((nErr = pthread_create(&clientTh, &clientThAttr, H_SERVER_WORK_TH, NULL)) < 0) {
            LOG (LOG_MAJ, "Err. Worker Thread Create Failed. Err.= '%s', idx=%d\n", strerror(nErr), i);
            return -1;
        }
    }

    return 0;
}
