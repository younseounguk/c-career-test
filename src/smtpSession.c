#include <string.h>

#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */

typedef struct _list_node {
    smtp_session_t* data;
    struct _list_node* next;
    int used;
} list_node;

list_node* head = NULL;
list_node* last = NULL;

// mutex 선언 및 초기화
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
    list_node* cur_node = head;
    list_node* prev_node = NULL;
    char* compare_id = NULL;
    
    LOG(LOG_DBG, "del start");

    pthread_mutex_lock(&mutex_lock);
    while(cur_node != NULL) {
        
        compare_id = cur_node->data->session_id;
        
        if(strcmp(session_id, compare_id) != 0) {
            prev_node = cur_node;
            cur_node = cur_node->next;
            continue;
        }
        
        // prev가 NULL이면 cur_node는 head
        if(prev_node == NULL) {
            if(cur_node->next != NULL) {
                // 다음 노드가 있으면 head 변경
                head = cur_node->next;
            }
            else {
                head = NULL;
            }
        }
        else {
            prev_node->next = cur_node->next;
        }

        if(cur_node == last) {
            // 현재 노드가 마지막 노드인 경우
            last = prev_node;
        }

        close(cur_node->data->sock_fd);
        free(cur_node);
        cur_node = NULL;
    }
    
    pthread_mutex_unlock(&mutex_lock);
    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */
    pthread_mutex_lock(&mutex_lock);

    list_node* new_node = (list_node*)malloc(sizeof(list_node));    
    memset(new_node, 0x00, sizeof(list_node));
    new_node->data = session;
    new_node->next = NULL;

    // 최초 추가일경우 헤더 생성
    if (head == NULL) {
        head = new_node;
        last = new_node;
    }
    else {
        last->next = new_node;
        last = new_node;    
    }

    pthread_mutex_unlock(&mutex_lock);
    return new_node->data;
}

smtp_session_t *getSmtpSession(char *session_id) {
    /*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
     *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
     *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
     */

    list_node* cur_node;
    char* compare_id = NULL;

    // 세션아이디가 없거나 저장한 값이 없다면 NULL 반환
    if(session_id == NULL || head == NULL || head->next == NULL) {
        return NULL; 
    }

    cur_node = head->next;
 
    while(cur_node != NULL) {
        compare_id = cur_node->data->session_id;
        if(strcmp(session_id, compare_id) == 0) {
	    return cur_node->data;
        }
	cur_node = cur_node->next;
    }   

    return NULL;
}
